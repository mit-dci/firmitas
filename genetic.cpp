#include <algorithm>

#include "genetic.h"

Market::geneticStrategy::geneticStrategy(Market* thisMarket, std::vector<uint64_t> accountIds, unsigned int nChildren)
{
    arma::vec initialMatrix(1000, arma::fill::randn);

    for(unsigned int i = 0; i < nChildren; i++)
    {
        initialMatrix.randn();
        generation temp;
        temp.accountId = accountIds[i];
        temp.score = 0;
        temp.startState = thisMarket->getAccount(temp.accountId);
        temp.strategyMatrix = initialMatrix;
        temp.startPrice = thisMarket->getCurrentPrice();
        children.push_back(temp);
    }

    market = thisMarket;
}

void Market::geneticStrategy::executeStrategy(std::vector<double> prices)
{
    arma::vec pricesMatrix(1000);
    for(unsigned int i = 0; i < prices.size(); i++)
    {
        pricesMatrix(i) = prices[i];
    }

    for(std::vector<generation>::iterator it = children.begin(); it < children.end(); it++)
    {
        arma::mat strat = (*it).strategyMatrix;
        double answer = arma::dot(pricesMatrix, strat);

        if(answer > 50)
        {
            market->clearOrders((*it).accountId);
            Market::account tempAccount = market->getAccount((*it).accountId);
            market->makeTrade(true, tempAccount.currencyBalance / market->getCurrentPrice(), market->getCurrentPrice(), tempAccount.id);
        }
        else if(answer < -50)
        {
            market->clearOrders((*it).accountId);
            Market::account tempAccount = market->getAccount((*it).accountId);
            market->makeTrade(false, tempAccount.commodityBalance, market->getCurrentPrice(), tempAccount.id);
        }
    }
}

void Market::geneticStrategy::evolve()
{
    generation best;
    best.score = 0;

    //Score each child
    for(std::vector<generation>::iterator it = children.begin(); it < children.end(); it++)
    {
        Market::account tempAccount = market->getAccount((*it).accountId);

        double currencyValue = tempAccount.currencyBalance;
        currencyValue += (tempAccount.commodityBalance * market->getCurrentPrice());

        double oldCurrencyValue = (*it).startState.currencyBalance;
        oldCurrencyValue += ((*it).startState.commodityBalance * (*it).startPrice);

        (*it).score = currencyValue / oldCurrencyValue;
        if(best.score < (*it).score)
        {
            best = *it;
        }
    }

    //Select best child and mutate it
    for(std::vector<generation>::iterator it = children.begin(); it < children.end(); it++)
    {
        std::uniform_int_distribution<unsigned int> startPosDist(0, 998);
        unsigned int startPos = startPosDist(market->generator);

        std::uniform_int_distribution<unsigned int> endPosDist(startPos + 1, std::min(startPos + 51, static_cast<unsigned int>(999)));
        unsigned int endPos = endPosDist(market->generator);

        std::uniform_real_distribution<double> valuesDist(-100, 100);

        arma::mat newMatrix = best.strategyMatrix;

        for(unsigned int pos = startPos; pos < endPos; pos++)
        {
            newMatrix(pos) = valuesDist(market->generator);
        }

        //std::cout << newMatrix << std::endl;

        (*it).strategyMatrix = newMatrix;
        (*it).startPrice = market->getCurrentPrice();
        (*it).startState = market->getAccount((*it).accountId);
    }
}
