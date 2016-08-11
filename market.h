#ifndef MARKET_H_INCLUDED
#define MARKET_H_INCLUDED

#include <string>
#include <map>
#include <random>

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
            double open;
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
            double sma200;
            double sma5;
            double rsi2;
            double averageGain;
            double averageLoss;
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
        std::map<uint64_t, account> accounts;
        uint64_t time;
        double currentPrice;
        candle currentDailyCandle;
        candle currentWeeklyCandle;
        account getAccountBalances(account Account);
        void executeOrder(bool buy, double amount, double price, uint64_t account1, uint64_t account2);
        double calculateInterestRate(candle currentCandle, std::map<uint64_t, candle> &candleList);
        double calculateBlockReward(candle currentCandle, std::map<uint64_t, candle> &candleList);
        candle calculateCandle(candle thisCandle, std::map<uint64_t, candle> &candleList);
        void processCCIStrategy(account& Account);
        void processRSI2Strategy(account& Account);
        void processRandomStrategy(account& Account);
        std::default_random_engine generator;
};

#endif // MARKET_H_INCLUDED
