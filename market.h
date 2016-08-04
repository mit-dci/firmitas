#ifndef MARKET_H_INCLUDED
#define MARKET_H_INCLUDED

#include <string>
#include <map>

class Market
{
    public:
        Market();
        ~Market();
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
        };
        struct account
        {
            uint64_t id;
            double commodityBalance;
            double currencyBalance;
        };
        bool makeTrade(bool buy, double amount, double price, uint64_t accountId);

    private:
        std::map<uint64_t, order> orderBook;
        std::map<uint64_t, candle> dailyCandles;
        std::map<uint64_t, account> accounts;
        uint64_t time;
        double currentPrice;
        candle currentCandle;
};

#endif // MARKET_H_INCLUDED
