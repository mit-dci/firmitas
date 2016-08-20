#include <vector>
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>
#include <sstream>

#include "market.h"

int main()
{
    Market market;

    return 0;
}

Market::Market()
{
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    time = 0;
    currentPrice = 1;
    currentDailyCandle.id = 0;
    currentDailyCandle.high = currentPrice;
    currentDailyCandle.low = currentPrice;
    currentDailyCandle.close = currentPrice;
    currentDailyCandle.up = false;
    currentDailyCandle.timestamp = time;
    currentDailyCandle.interestRate = 1;
    currentDailyCandle.sma100 = 1;
    currentDailyCandle.sma20 = 1;
    currentDailyCandle.sma5 = 1;
    currentDailyCandle.sma200 = 1;
    currentDailyCandle.tpsma26 = 1;
    currentDailyCandle.cci26 = 0;
    currentDailyCandle.volumeCommodity = 0;
    currentDailyCandle.volumeCurrency = 0;
    currentDailyCandle.inflation = 1;
    currentDailyCandle.typicalPrice = 1;
    currentDailyCandle.averageGain = 0;
    currentDailyCandle.averageLoss = 0;
    currentDailyCandle.open = 1;
    dailyCandles.insert(std::pair<uint64_t, candle>(currentDailyCandle.id, currentDailyCandle));

    currentWeeklyCandle.id = 0;
    currentWeeklyCandle.high = currentPrice;
    currentWeeklyCandle.low = currentPrice;
    currentWeeklyCandle.close = currentPrice;
    currentWeeklyCandle.up = false;
    currentWeeklyCandle.timestamp = time;
    currentWeeklyCandle.interestRate = 1;
    currentWeeklyCandle.sma100 = 1;
    currentWeeklyCandle.sma20 = 1;
    currentWeeklyCandle.sma5 = 1;
    currentWeeklyCandle.sma200 = 1;
    currentWeeklyCandle.tpsma26 = 1;
    currentWeeklyCandle.cci26 = 0;
    currentWeeklyCandle.volumeCommodity = 0;
    currentWeeklyCandle.volumeCurrency = 0;
    currentWeeklyCandle.blockReward = 1;
    currentWeeklyCandle.inflation = 1;
    currentWeeklyCandle.typicalPrice = 1;
    currentWeeklyCandle.averageGain = 0;
    currentWeeklyCandle.averageLoss = 0;
    currentWeeklyCandle.open = 1;
    weeklyCandles.insert(std::pair<uint64_t, candle>(currentWeeklyCandle.id, currentWeeklyCandle));

    currentMonthlyCandle.id = 0;
    currentMonthlyCandle.high = currentPrice;
    currentMonthlyCandle.low = currentPrice;
    currentMonthlyCandle.close = currentPrice;
    currentMonthlyCandle.up = false;
    currentMonthlyCandle.timestamp = time;
    currentMonthlyCandle.interestRate = 1;
    currentMonthlyCandle.sma100 = 1;
    currentMonthlyCandle.sma20 = 1;
    currentMonthlyCandle.sma5 = 1;
    currentMonthlyCandle.sma200 = 1;
    currentMonthlyCandle.tpsma26 = 1;
    currentMonthlyCandle.cci26 = 0;
    currentMonthlyCandle.volumeCommodity = 0;
    currentMonthlyCandle.volumeCurrency = 0;
    currentMonthlyCandle.blockReward = 1;
    currentMonthlyCandle.inflation = 1;
    currentMonthlyCandle.typicalPrice = 1;
    currentMonthlyCandle.averageGain = 0;
    currentMonthlyCandle.averageLoss = 0;
    currentMonthlyCandle.open = 1;
    monthlyCandles.insert(std::pair<uint64_t, candle>(currentMonthlyCandle.id, currentMonthlyCandle));

    time++;

    std::uniform_real_distribution<double> distribution(0, 316);

    for(unsigned int i = 0; i < 950; i++)
    {
        account Account;
        Account.id = i;
        Account.commodityBalance = distribution(generator);
        Account.currencyBalance = distribution(generator);
        accounts.insert(std::pair<uint64_t, account>(Account.id, Account));
    }

    std::ofstream candleFile;
    std::stringstream filename;
    filename << std::chrono::system_clock::now().time_since_epoch().count() << "_candles.csv";
    candleFile.open(filename.str());

    while(time <= 10 * 365 * 24 * 60 * 60)
    {
        //Make daily candle
        if(time % (60 * 60 * 24) == 0)
        {
            currentDailyCandle = calculateCandle(currentDailyCandle, dailyCandles);
            dailyCandles.insert(std::pair<uint64_t, candle>(currentDailyCandle.id, currentDailyCandle));

            std::cout << "Daily Candle: " << currentDailyCandle.id << ", High: " << currentDailyCandle.high << ", Low: " << currentDailyCandle.low << ", Close: " << currentDailyCandle.close << ", Up: " << currentDailyCandle.up << ", Interest rate: " << (currentWeeklyCandle.interestRate - 1) * 100 << "%, SMA(100): $" << currentDailyCandle.sma100 << ", SMA(20): $" << currentDailyCandle.sma20 << ", commodityVolume: " << currentDailyCandle.volumeCommodity << ", currencyVolume: " << currentDailyCandle.volumeCurrency << ", CCI(26): " << currentDailyCandle.cci26 << ", Inflation: " << (1 - currentDailyCandle.inflation) * 100 << "%, Block reward: " << currentDailyCandle.blockReward << ", RSI(2): " << currentDailyCandle.rsi2 << std::endl;

            time_t tt = time;
            struct tm * ptm = std::localtime(&tt);
            char buf[9];
            std::strftime(buf, 9, "%Y%m%d", ptm);
            candleFile << buf << "," << currentDailyCandle.open << "," << currentDailyCandle.close << "," << currentDailyCandle.high << "," << currentDailyCandle.low << "," << currentDailyCandle.volumeCommodity << "\n";

            currentDailyCandle.high = currentPrice;
            currentDailyCandle.low = currentPrice;
            currentDailyCandle.volumeCommodity = 0;
            currentDailyCandle.volumeCurrency = 0;
            currentDailyCandle.open = currentDailyCandle.close;

            for(std::map<uint64_t, account>::iterator it = accounts.begin(); it != accounts.end(); it++)
            {
                it->second.commodityBalance *= currentWeeklyCandle.interestRate;
            }

            for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
            {
                if(!it->second.buy)
                {
                    it->second.amount *= currentWeeklyCandle.interestRate;
                }
            }

            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if(time % (30 * 24 * 60 * 60) == 0)
        {
            currentMonthlyCandle = calculateCandle(currentMonthlyCandle, monthlyCandles);
            monthlyCandles.insert(std::pair<uint64_t, candle>(currentMonthlyCandle.id, currentMonthlyCandle));

            //std::cout << "Monthly Candle: " << currentMonthlyCandle.id << ", High: " << currentMonthlyCandle.high << ", Low: " << currentMonthlyCandle.low << ", Close: " << currentMonthlyCandle.close << ", Up: " << currentMonthlyCandle.up << ", Interest rate: " << (currentMonthlyCandle.interestRate - 1) * 100 << "%, SMA(100): $" << currentMonthlyCandle.sma100 << ", SMA(20): $" << currentMonthlyCandle.sma20 << ", commodityVolume: " << currentMonthlyCandle.volumeCommodity << ", currencyVolume: " << currentMonthlyCandle.volumeCurrency << ", CCI(26): " << currentMonthlyCandle.cci26 << ", Inflation: " << (1 - currentMonthlyCandle.inflation) * 100 << "%, Block reward: " << currentMonthlyCandle.blockReward << std::endl;

            currentMonthlyCandle.high = currentPrice;
            currentMonthlyCandle.low = currentPrice;
            currentMonthlyCandle.volumeCommodity = 0;
            currentMonthlyCandle.volumeCurrency = 0;
            currentMonthlyCandle.open = currentMonthlyCandle.close;
        }

        if(time % (7 * 24 * 60 * 60) == 0)
        {
            currentWeeklyCandle = calculateCandle(currentWeeklyCandle, weeklyCandles);
            weeklyCandles.insert(std::pair<uint64_t, candle>(currentWeeklyCandle.id, currentWeeklyCandle));

            //std::cout << "Weekly Candle: " << currentWeeklyCandle.id << ", High: " << currentWeeklyCandle.high << ", Low: " << currentWeeklyCandle.low << ", Close: " << currentWeeklyCandle.close << ", Up: " << currentWeeklyCandle.up << ", Interest rate: " << (currentWeeklyCandle.interestRate - 1) * 100 << "%, SMA(100): $" << currentWeeklyCandle.sma100 << ", SMA(20): $" << currentWeeklyCandle.sma20 << ", commodityVolume: " << currentWeeklyCandle.volumeCommodity << ", currencyVolume: " << currentWeeklyCandle.volumeCurrency << ", CCI(26): " << currentWeeklyCandle.cci26 << ", Inflation: " << (1 - currentWeeklyCandle.inflation) * 100 << "%, Block reward: " << currentWeeklyCandle.blockReward << ", Total currency: $" << getTotalCurrency() << ", Total commodity: " << getTotalCommodity() << std::endl;

            currentWeeklyCandle.high = currentPrice;
            currentWeeklyCandle.low = currentPrice;
            currentWeeklyCandle.volumeCommodity = 0;
            currentWeeklyCandle.volumeCurrency = 0;
            currentWeeklyCandle.open = currentWeeklyCandle.close;
        }

        /*if(time % (30 * 7 * 24 * 60 * 60) == 0)
        {
            std::cout << "Monthly inflation: " << (1 - (currentWeeklyCandle.sma20 / weeklyCandles[currentWeeklyCandle.id - 4].sma20)) * 100 << "%\n";
        }

        if(time % (365 * 7 * 24 * 60 * 60) == 0)
        {
            std::cout << "Yearly inflation: " << (1 - (currentWeeklyCandle.sma20 / weeklyCandles[currentWeeklyCandle.id - 54].sma20)) * 100 << "%\n";
        }*/

        std::uniform_int_distribution<uint64_t> accountIdDistribution(0, 950);

        if(time % 15 == 0)
        {
            account Account = accounts[accountIdDistribution(generator)];
            if(Account.id < 317)
            {
                processRandomStrategy(Account);
            }
            else if(Account.id < 658)
            {
                processCCIStrategy(Account);
            }
            else
            {
                processRSI2Strategy(Account);
            }
        }

        std::uniform_real_distribution<double> growthDistribution(0, 54);
        std::uniform_int_distribution<uint64_t> blockRewardIdDistribution(0, accounts.size() - 1);

        /*if(time % (2 * 365 * 24 * 60 * 60) == 0)
        {
            blockReward /= 2;
        }*/

        //Simulate mining
        if(time % 150 == 0)
        {
            //accounts[blockRewardIdDistribution(generator)].commodityBalance += blockReward;
            accounts[blockRewardIdDistribution(generator)].commodityBalance += currentDailyCandle.blockReward;
            accounts[accountIdDistribution(generator)].currencyBalance += growthDistribution(generator);
        }

        //Cancel stale orders
        for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
        {
            if((time - it->second.timestamp) > 5 * 24 * 60 * 60)
            {
                it = orderBook.erase(it);
                if(orderBook.size() < 1)
                {
                    break;
                }
            }
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

        time++;
    }

    candleFile.close();

    for(std::map<uint64_t, account>::iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        std::cout << "Account " << it->second.id << ", currency: $" << it->second.currencyBalance << ", commodity: " << it->second.commodityBalance << std::endl;
    }

    for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
    {
        if(it->second.buy)
        {
            std::cout << "Account " << it->second.accountId << " buying, amount: " << it->second.amount << ", price: $" << it->second.price << std::endl;
        }
        else
        {
            std::cout << "Account " << it->second.accountId << " selling, amount: " << it->second.amount << ", price: $" << it->second.price << std::endl;
        }
    }
}

Market::candle Market::calculateCandle(candle thisCandle, std::map<uint64_t, candle> &candleList)
{
    candle currentCandle = thisCandle;

    currentCandle.id++;
    currentCandle.close = currentPrice;
    if(currentCandle.close > candleList[currentCandle.id - 1].close)
    {
        currentCandle.up = true;
    }
    else
    {
        currentCandle.up = false;
    }

    currentCandle.timestamp = time;

    currentCandle.interestRate = calculateInterestRate(currentCandle, candleList);
    currentCandle.blockReward = calculateBlockReward(currentCandle, candleList);

    //Calculate 100-period SMA
    double total = currentCandle.close;
    unsigned int i;
    for(i = 1; i < 100; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += candleList[currentCandle.id - i].close;
    }
    currentCandle.sma100 = total / i;

    //Calculate 20-period SMA
    total = currentCandle.close;
    for(i = 1; i < 20; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += candleList[currentCandle.id - i].close;
    }
    currentCandle.sma20 = total / i;

    //Calculate 5-period SMA
    total = currentCandle.close;
    for(i = 1; i < 5; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += candleList[currentCandle.id - i].close;
    }
    currentCandle.sma5 = total / i;

    //Calculate 200-period SMA
    total = currentCandle.close;
    for(i = 1; i < 200; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += candleList[currentCandle.id - i].close;
    }
    currentCandle.sma200 = total / i;

    //Calculate 2-period RSI
    if(currentCandle.id < 2)
    {
        if(currentCandle.up)
        {
            currentCandle.averageGain = currentCandle.close - 1;
        }
        else
        {
            currentCandle.averageLoss = 1 - currentCandle.close;
        }
    }
    else
    {
        if(currentCandle.up)
        {
            currentCandle.averageGain = (currentCandle.averageGain + (currentCandle.close - candleList[currentCandle.id - 1].close)) / 2;
            currentCandle.averageLoss /= 2;
        }
        else
        {
            currentCandle.averageLoss = (currentCandle.averageLoss + (candleList[currentCandle.id - 1].close - currentCandle.close)) / 2;
            currentCandle.averageGain /= 2;
        }
    }

    if(currentCandle.averageLoss <= 0)
    {
        currentCandle.rsi2 = 100;
    }
    else
    {
        currentCandle.rsi2 = 100 - (100 / (1 + (currentCandle.averageGain / currentCandle.averageLoss)));
    }

    //Calculate typical price
    currentCandle.typicalPrice = (currentCandle.low + currentCandle.high + currentCandle.close) / 3;

    //Calculate 26-period typical price SMA
    total = currentCandle.typicalPrice;
    for(i = 1; i < 26; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += candleList[currentCandle.id - i].typicalPrice;
    }
    currentCandle.tpsma26 = total / i;

    //Calculate mean deviation
    total = std::abs(currentCandle.tpsma26 - currentCandle.typicalPrice);
    for(i = 1; i < 26; i++)
    {
        int nextId = currentCandle.id - i;
        if(nextId < 0)
        {
            break;
        }
        total += std::abs(currentCandle.tpsma26 - candleList[currentCandle.id - i].typicalPrice);
    }
    double meanDeviation = total / i;

    //Calculate 20-period CCI
    currentCandle.cci26 = (currentCandle.typicalPrice - currentCandle.tpsma26) / (0.015 * meanDeviation);

    currentCandle.inflation = currentCandle.typicalPrice / candleList[currentCandle.id - 1].typicalPrice;

    return currentCandle;
}

Market::account Market::getAccountBalances(account Account)
{
    double commodityBalance = Account.commodityBalance;
    double currencyBalance = Account.currencyBalance;

    for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
    {
        if(it->second.buy && it->second.accountId == Account.id)
        {
            currencyBalance -= it->second.amount * it->second.price;
        }
        else if(!it->second.buy && it->second.accountId == Account.id)
        {
            commodityBalance -= it->second.amount;
        }
    }

    account returning;
    returning.commodityBalance = commodityBalance;
    returning.currencyBalance = currencyBalance;
    returning.id = Account.id;

    return returning;
}

void Market::executeOrder(bool buy, double amount, double price, uint64_t account1, uint64_t account2)
{
    if(buy)
    {
        accounts[account1].currencyBalance += (price * amount);
        accounts[account1].commodityBalance -= amount;

        accounts[account2].currencyBalance -= (price * amount);
        accounts[account2].commodityBalance += amount;

        //std::cout << account2 << " bought from " << account1 << " " << amount << " of commodity for $" << price << std::endl;
    }
    else
    {
        accounts[account1].commodityBalance += amount;
        accounts[account1].currencyBalance -= (price * amount);

        accounts[account2].commodityBalance -= amount;
        accounts[account2].currencyBalance += (price * amount);

        //std::cout << account2 << " sold to " << account1 << " " << amount << " of commodity for $" << price << std::endl;
    }

    currentDailyCandle.volumeCommodity += amount;
    currentDailyCandle.volumeCurrency += amount * price;

    currentWeeklyCandle.volumeCommodity += amount;
    currentWeeklyCandle.volumeCurrency += amount * price;

    currentMonthlyCandle.volumeCommodity += amount;
    currentMonthlyCandle.volumeCurrency += amount * price;
}

bool Market::makeTrade(bool buy, double amount, double price, uint64_t accountId)
{
    account Account = getAccountBalances(accounts[accountId]);

    if(((Account.commodityBalance < amount || Account.commodityBalance < 0.00000001) && !buy) || ((Account.currencyBalance < amount * price || Account.currencyBalance < 0.01) && buy) || price < 0.01 || amount <= 0.00000001)
    {
        return false;
    }

    std::vector<order> tempOrders;

    //Find orders which can at least partially fill the order
    for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
    {
        if((it->second.price <= price && buy && !it->second.buy) || (it->second.price >= price && !buy && it->second.buy))
        {
            tempOrders.push_back(it->second);
        }
    }

    double newAmount = amount;

    if(!tempOrders.empty())
    {
        //Fill lowest price order first if buying, highest first if selling
        if(buy)
        {
            std::sort(tempOrders.begin(), tempOrders.end(), [](order first, order second)
            {
                return (first.price < second.price && first.timestamp < second.timestamp);
            });
        }
        else
        {
            std::sort(tempOrders.begin(), tempOrders.end(), [](order first, order second)
            {
                return (first.price > second.price && first.timestamp > second.timestamp);
            });
        }

        unsigned int i = 0;
        while(newAmount > 0 && i < tempOrders.size())
        {
            if(tempOrders[i].amount <= newAmount)
            {
                newAmount -= tempOrders[i].amount;

                //Remove order from orderbook
                orderBook.erase(tempOrders[i].id);

                executeOrder(buy, tempOrders[i].amount, tempOrders[i].price, tempOrders[i].accountId, accountId);
            }
            else
            {
                orderBook[tempOrders[i].id].amount -= newAmount;

                executeOrder(buy, newAmount, tempOrders[i].price, tempOrders[i].accountId, accountId);

                newAmount = 0;
            }

            currentPrice = tempOrders[i].price;
            if(currentPrice > currentDailyCandle.high)
            {
                currentDailyCandle.high = currentPrice;
            }
            if(currentPrice < currentDailyCandle.low)
            {
                currentDailyCandle.low = currentPrice;
            }

            if(currentPrice > currentWeeklyCandle.high)
            {
                currentWeeklyCandle.high = currentPrice;
            }
            if(currentPrice < currentWeeklyCandle.low)
            {
                currentWeeklyCandle.low = currentPrice;
            }

            if(currentPrice > currentMonthlyCandle.high)
            {
                currentMonthlyCandle.high = currentPrice;
            }
            if(currentPrice < currentMonthlyCandle.low)
            {
                currentMonthlyCandle.low = currentPrice;
            }

            i++;
        }
    }

    if(newAmount > 0)
    {
        order newOrder;
        newOrder.amount = newAmount;
        newOrder.price = price;
        newOrder.accountId = accountId;
        newOrder.buy = buy;
        newOrder.timestamp = time;

        std::default_random_engine generator(time);
        std::uniform_int_distribution<uint64_t> distribution(0, (2^64) - 1);
        newOrder.id = distribution(generator);

        orderBook.insert(std::pair<uint64_t, order>(newOrder.id, newOrder));
    }

    return true;
}

double Market::calculateInterestRate(candle currentCandle, std::map<uint64_t, candle> &candleList)
{
    /*const uint64_t minDays = 1;
    const uint64_t maxDays = 30;*/
    const double minRate = 0.99;
    const double maxRate = 1.01;

    /*candle thisCandle = candleList[currentCandle.id - 1];

    if(thisCandle.id < minDays)
    {
        return 1;
    }
    else
    {
        uint64_t candlesScanned = 0;
        double interestAverage = 1;
        double previousInterestAverage = 1;
        double actualRate = 0;
        double targetRate = 0;
        double rateAdjustmentRatio = 1.0;
        double eventHorizonDeviation = 0.0;
        double eventHorizonDeviationFast = 0.0;
        double eventHorizonDeviationSlow = 0.0;

        for(unsigned int i = 1; thisCandle.id != 0; i++)
        {
            if(i > maxDays)
            {
                break;
            }

            candlesScanned++;

            if(i == 1)
            {
                interestAverage = thisCandle.interestRate;
            }
            else
            {
                interestAverage = ((thisCandle.interestRate - previousInterestAverage) / double(i)) + previousInterestAverage;
            }

            previousInterestAverage = interestAverage;

            actualRate += thisCandle.typicalPrice;
            targetRate = candlesScanned;
            rateAdjustmentRatio = 1.0;

            if(actualRate < 0)
            {
                actualRate = 0;
            }

            if(actualRate != 0 && targetRate != 0)
            {
                rateAdjustmentRatio = targetRate / actualRate;
            }

            eventHorizonDeviation = 1 + (0.7084 * pow((double(candlesScanned)/double(minDays)), -1.228));
            eventHorizonDeviationFast = eventHorizonDeviation;
            eventHorizonDeviationSlow = 1 / eventHorizonDeviation;

            if(candlesScanned >= minDays)
            {
                if((rateAdjustmentRatio <= eventHorizonDeviationSlow) || (rateAdjustmentRatio >= eventHorizonDeviationFast))
                {
                    break;
                }
            }

            if(thisCandle.id == 1)
            {
                break;
            }
            thisCandle = candleList[thisCandle.id - 1];
        }

        double newRate = interestAverage;
        if(actualRate != 0 && targetRate != 0)
        {
            newRate *= actualRate;

            newRate /= targetRate;
        }

        if(newRate < minRate)
        {
            newRate = minRate;
        }
        if(newRate > maxRate)
        {
            newRate = maxRate;
        }

        return newRate;
    }*/

    double newRate = std::pow(currentCandle.typicalPrice + currentCandle.inflation - 1, 0.33333);

    if(newRate < minRate)
    {
        newRate = minRate;
    }
    if(newRate > maxRate)
    {
        newRate = maxRate;
    }

    //return newRate;
    return 1;
}

double Market::calculateBlockReward(candle currentCandle, std::map<uint64_t, candle> &candleList)
{
    candle thisCandle = candleList[currentCandle.id - 1];

    //double dP = thisCandle.sma5 - candleList[thisCandle.id - 1].sma5;
    //double ddP = dP - (candleList[thisCandle.id - 1].sma5 - candleList[thisCandle.id - 2].sma5);

    const unsigned int maxLookback = 365;

    const double kP = 1;
    const double kI = 2.5;
    const double kD = 0.25;

    double error = thisCandle.typicalPrice - 1;
    double integral = 0;

    for(unsigned int i = 0; i < maxLookback; i++)
    {
        integral += (candleList[thisCandle.id - i].typicalPrice - 1);
        if(thisCandle.id - i <= 0)
        {
            break;
        }
    }

    double derivative = error - (candleList[thisCandle.id - 1].typicalPrice - 1);

    double newTarget = (kP * error) + (kI * integral) + (kD * derivative);

    if(newTarget < 0)
    {
        newTarget = 0;
    }
    if(newTarget > 1500)
    {
        newTarget = 1500;
    }

    return newTarget;

    /*if(thisCandle.id < minDays)
    {
        return 1500;
    }
    else
    {
        uint64_t candlesScanned = 0;
        double interestAverage = 1;
        double previousInterestAverage = 1;
        double actualRate = 0;
        double targetRate = 0;
        double rateAdjustmentRatio = 1.0;
        double eventHorizonDeviation = 0.0;
        double eventHorizonDeviationFast = 0.0;
        double eventHorizonDeviationSlow = 0.0;

        for(unsigned int i = 1; thisCandle.id != 0; i++)
        {
            if(i > maxDays)
            {
                break;
            }

            candlesScanned++;

            if(i == 1)
            {
                interestAverage = thisCandle.blockReward;
            }
            else
            {
                interestAverage = ((thisCandle.blockReward - previousInterestAverage) / double(i)) + previousInterestAverage;
            }

            previousInterestAverage = interestAverage;

            actualRate += thisCandle.typicalPrice;
            targetRate = candlesScanned;
            rateAdjustmentRatio = 1.0;

            if(actualRate < 0)
            {
                actualRate = 0;
            }

            if(actualRate != 0 && targetRate != 0)
            {
                rateAdjustmentRatio = targetRate / actualRate;
            }

            eventHorizonDeviation = 1 + (0.7084 * pow((double(candlesScanned)/double(minDays)), -1.228));
            eventHorizonDeviationFast = eventHorizonDeviation;
            eventHorizonDeviationSlow = 1 / eventHorizonDeviation;

            if(candlesScanned >= minDays)
            {
                if((rateAdjustmentRatio <= eventHorizonDeviationSlow) || (rateAdjustmentRatio >= eventHorizonDeviationFast))
                {
                    break;
                }
            }

            if(thisCandle.id == 1)
            {
                break;
            }
            thisCandle = candleList[thisCandle.id - 1];
        }

        double newRate = interestAverage;
        if(actualRate != 0 && targetRate != 0)
        {
            newRate *= actualRate;

            newRate /= targetRate;
        }

        if(newRate < minRate)
        {
            newRate = minRate;
        }
        if(newRate > maxRate)
        {
            newRate = maxRate;
        }

        return newRate;
    }*/
}

double Market::getTotalCommodity()
{
    double commodity = 0;
    for(std::map<uint64_t, account>::iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        commodity += it->second.commodityBalance;
    }

    return commodity;
}

double Market::getTotalCurrency()
{
    double currency = 0;
    for(std::map<uint64_t, account>::iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        currency += it->second.currencyBalance;
    }

    return currency;
}
