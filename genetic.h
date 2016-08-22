#ifndef GENETIC_H_INCLUDED
#define GENETIC_H_INCLUDED

#include <vector>

#include <armadillo>

#include "market.h"

class Market::geneticStrategy
{
    public:
        geneticStrategy(Market* thisMarket, std::vector<uint64_t> accountIds, unsigned int nChildren = 3);
        void executeStrategy(std::vector<double> prices);
        void evolve();

    private:
        struct generation
        {
            arma::mat strategyMatrix;
            double score;
            uint64_t accountId;
            Market::account startState;
            double startPrice;
        };
        std::vector<generation> children;
        Market* market;
};

#endif // GENETIC_H_INCLUDED
