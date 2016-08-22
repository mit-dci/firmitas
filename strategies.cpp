#include "market.h"

void Market::processCCIStrategy(account& Account)
{
    //Check bias
    if((currentWeeklyCandle.cci26 < 0 && weeklyCandles[currentWeeklyCandle.id - 1].cci26 > 0) || (currentWeeklyCandle.cci26 > 0 && weeklyCandles[currentWeeklyCandle.id - 1].cci26 < 0))
    {
        Account.weeklycciPassed0 = true;
    }

    if(Account.bullishBias && Account.weeklycciPassed0 && currentWeeklyCandle.cci26 < -100)
    {
        Account.bullishBias = false;
        Account.weeklycciPassed0 = false;
    }

    if(!Account.bullishBias && Account.weeklycciPassed0 && currentWeeklyCandle.cci26 > 100)
    {
        Account.bullishBias = true;
        Account.weeklycciPassed0 = false;
    }

    //Check trade
    if(Account.bullishBias && currentDailyCandle.cci26 < -100)
    {
        Account.dailyOversold = true;
    }

    if(!Account.bullishBias && currentDailyCandle.cci26 > 100)
    {
        Account.dailyOversold = false;
    }

    if(Account.bullishBias && Account.dailyOversold && currentDailyCandle.cci26 < 0 && dailyCandles[currentDailyCandle.id - 1].cci26 > 0)
    {
        clearOrders(Account.id);
        Account = getAccountBalances(Account);
        makeTrade(true, (Account.currencyBalance / currentPrice), currentPrice, Account.id);
    }

    if(!Account.bullishBias && !Account.dailyOversold && currentDailyCandle.cci26 > 0 && dailyCandles[currentDailyCandle.id - 1].cci26 < 0)
    {
        clearOrders(Account.id);
        Account = getAccountBalances(Account);
        makeTrade(false, Account.commodityBalance, currentPrice, Account.id);
    }
}

void Market::processRandomStrategy(account& Account)
{
    Account = getAccountBalances(Account);
    std::uniform_real_distribution<double> commodityBalanceDistribution(0.0000001, Account.commodityBalance);
    std::uniform_real_distribution<double> currencyBalanceDistribution(0.01, Account.currencyBalance);
    std::uniform_real_distribution<double> priceDistribution(std::max(currentPrice * 0.999, 0.01), currentPrice * 1.001);
    std::uniform_int_distribution<unsigned int> buySellDist(0, 1);
    double tempPrice = priceDistribution(generator);
    if(buySellDist(generator))
    {
        makeTrade(true, currencyBalanceDistribution(generator) / tempPrice, tempPrice, Account.id);
    }
    else
    {
        makeTrade(false, commodityBalanceDistribution(generator), tempPrice, Account.id);
    }
}

void Market::processRSI2Strategy(account& Account)
{
    bool trading = false;
    bool buy = false;

    if(currentDailyCandle.sma200 > currentDailyCandle.sma5 && currentDailyCandle.rsi2 >= 95)
    {
        trading = true;
    }
    else if(currentDailyCandle.sma200 < currentDailyCandle.sma5 && currentDailyCandle.rsi2 <= 5)
    {
        trading = true;
        buy = true;
    }

    if(trading)
    {
        clearOrders(Account.id);

        Account = getAccountBalances(Account);

        if(buy)
        {
            makeTrade(true, (Account.currencyBalance / currentPrice), currentPrice, Account.id);
        }
        else
        {
            makeTrade(false, Account.commodityBalance, currentPrice, Account.id);
        }
    }
}
