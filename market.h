#ifndef MARKET_H_INCLUDED
#define MARKET_H_INCLUDED

#include <string>
#include <map>

class Market
{
    public:
        Market();
        struct order
        {
            uint64_t id;
            double amount;
            double price;
            uint64_t accountId;
            uint64_t timestamp;
            bool buy;
        };
        struct candle
        {
            uint64_t id;
            uint64_t timestamp;
            double high;
            double low;
            double close;
            bool up;
            double interestRate;
            double sma100;
            double sma20;
            double tpsma26;
            double typicalPrice;
            double cci26;
            double volumeCommodity;
            double volumeCurrency;
            double inflation;
            double blockReward;
        };
        struct account
        {
            uint64_t id;
            double commodityBalance;
            double currencyBalance;
            bool bullishBias;
            bool weeklycciPassed0;
            bool dailyOversold;
        };
        bool makeTrade(bool buy, double amount, double price, uint64_t accountId);

    private:
        std::map<uint64_t, order> orderBook;
        std::map<uint64_t, candle> dailyCandles;
        std::map<uint64_t, candle> weeklyCandles;
        std::map<uint64_t, candle> monthlyCandles;
        std::map<uint64_t, account> accounts;
        uint64_t time;
        double currentPrice;
        candle currentDailyCandle;
        candle currentWeeklyCandle;
        candle currentMonthlyCandle;
        account getAccountBalances(account Account);
        void executeOrder(bool buy, double amount, double price, uint64_t account1, uint64_t account2);
        double calculateInterestRate(candle currentCandle, std::map<uint64_t, candle> &candleList);
        double calculateBlockReward(candle currentCandle, std::map<uint64_t, candle> &candleList);
        candle calculateCandle(candle thisCandle, std::map<uint64_t, candle> &candleList);
        void processCCIStrategy(account& Account);
};

#endif // MARKET_H_INCLUDED
