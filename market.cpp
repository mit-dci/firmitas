#include <vector>
#include <algorithm>
#include <stdint.h>
#include <iostream>

#include "market.h"

int main()
{
    Market market;

    return 0;
}

Market::Market()
{
    time = 0;
    currentPrice = 1;
    currentCandle.id = 0;
    currentCandle.high = currentPrice;
    currentCandle.low = currentPrice;
    currentCandle.close = currentPrice;
    currentCandle.up = false;
    currentCandle.timestamp = time;
    dailyCandles.insert(std::pair<uint64_t, candle>(currentCandle.id, currentCandle));
    time++;

    while(true)
    {
        //Make daily candle
        if(time % (60 * 60 * 24) == 0)
        {
            currentCandle.id++;
            currentCandle.close = currentPrice;
            if(currentCandle.close > dailyCandles[currentCandle.id - 1].close)
            {
                currentCandle.up = true;
            }
            else
            {
                currentCandle.up = false;
            }

            currentCandle.timestamp = time;
            dailyCandles.insert(std::pair<uint64_t, candle>(currentCandle.id, currentCandle));

            std::cout << "Candle: " << currentCandle.id << ", High: " << currentCandle.high << ", Low: " << currentCandle.low << ", Close: " << currentCandle.close << ", Up: " << currentCandle.up << std::endl;

            currentCandle.high = currentPrice;
            currentCandle.low = currentPrice;
        }

        time++;
    }
}

Market::~Market()
{

}

bool Market::makeTrade(bool buy, double amount, double price, uint64_t accountId)
{
    double commodityBalance = accounts[accountId].commodityBalance;
    double currencyBalance = accounts[accountId].currencyBalance;

    for(std::map<uint64_t, order>::iterator it = orderBook.begin(); it != orderBook.end(); it++)
    {
        if(it->second.buy && it->second.accountId == accountId)
        {
            currencyBalance -= it->second.amount * it->second.price;
        }
        else if(!it->second.buy && it->second.accountId == accountId)
        {
            commodityBalance -= it->second.amount;
        }
    }

    if((commodityBalance < amount && !buy) || (currencyBalance < amount && buy))
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

                //Credit order owner
                if(buy)
                {
                    accounts[tempOrders[i].accountId].currencyBalance += (tempOrders[i].price * tempOrders[i].amount);
                    accounts[tempOrders[i].accountId].commodityBalance -= tempOrders[i].amount;

                    accounts[accountId].currencyBalance -= (tempOrders[i].price * tempOrders[i].amount);
                    accounts[accountId].commodityBalance += tempOrders[i].amount;
                }
                else
                {
                    accounts[tempOrders[i].accountId].commodityBalance += tempOrders[i].amount;
                    accounts[tempOrders[i].accountId].currencyBalance -= (tempOrders[i].price * tempOrders[i].amount);

                    accounts[accountId].commodityBalance -= tempOrders[i].amount;
                    accounts[accountId].currencyBalance += (tempOrders[i].price * tempOrders[i].amount);
                }
            }
            else
            {
                orderBook[tempOrders[i].id].amount -= newAmount;

                if(buy)
                {
                    accounts[tempOrders[i].accountId].currencyBalance += (tempOrders[i].price * newAmount);
                    accounts[tempOrders[i].accountId].commodityBalance -= newAmount;

                    accounts[accountId].currencyBalance -= (tempOrders[i].price * newAmount);
                    accounts[accountId].commodityBalance += newAmount;
                }
                else
                {
                    accounts[tempOrders[i].accountId].currencyBalance -= (tempOrders[i].price * newAmount);
                    accounts[tempOrders[i].accountId].commodityBalance += newAmount;

                    accounts[accountId].currencyBalance += (tempOrders[i].price * newAmount);
                    accounts[accountId].commodityBalance -= newAmount;
                }

                newAmount = 0;
            }
            currentPrice = tempOrders[i].price;
            if(currentPrice > currentCandle.high)
            {
                currentCandle.high = currentPrice;
            }
            if(currentPrice < currentCandle.low)
            {
                currentCandle.low = currentPrice;
            }
            i++;
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
    else
    {
        order newOrder;
        newOrder.amount = amount;
        newOrder.price = price;
        newOrder.accountId = accountId;
        newOrder.buy = buy;
        newOrder.timestamp = time;

        std::default_random_engine generator(time);
        std::uniform_int_distribution<uint64_t> distribution(0, (2^64) - 1);
        newOrder.id = distribution(generator);

        orderBook.insert(std::pair<uint64_t, order>(newOrder.id, newOrder));

        return true;
    }
}
