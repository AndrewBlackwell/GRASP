#include "Trainer.hpp"
#include <iostream>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/unordered_map.hpp>
#include "Node.hpp"

namespace Trainer
{

    // @brief Constructs a Trainer object, initializing the game and loading strategies if provided.
    // @param mode The mode of CFR to use (e.g., standard, chance, external, outcome).
    // @param seed A seed for the random number generator.
    // @param strategyPaths Paths to pre-existing strategies for players, if any.
    template <typename Type>
    Trainer<Type>::Trainer(const std::string &mode, const uint32_t seed, const std::vector<std::string> &strategyPaths)
        : randomGenerator(seed), mNodeTouchedCnt(0), mModeStr(mode)
    {
        mGame = new Type(randomGenerator);
        mFolderPath = "../strategies/" + mGame->name();
        boost::filesystem::create_directories(mFolderPath);
        mFixedStrategies = new std::unordered_map<std::string, Node *>[mGame->playerNum()];
        mUpdate = new bool[mGame->playerNum()];
        for (int i = 0; i < mGame->playerNum(); ++i)
        {
            if (strategyPaths.size() >= i + 1 && !strategyPaths[i].empty())
            {
                std::cout << "load strategy \"" << strategyPaths[i] << "\" as static player " << i << std::endl;
                std::ifstream ifs(strategyPaths[i]);
                boost::archive::binary_iarchive ia(ifs);
                ia >> mFixedStrategies[i];
                ifs.close();
                mUpdate[i] = false;
            }
            else
            {
                mUpdate[i] = true;
            }
        }
    }

    // @brief Destructor for Trainer, responsible for cleaning up dynamically allocated memory.
    template <typename Type>
    Trainer<Type>::~Trainer()
    {
        for (auto &itr : mNodeMap)
        {
            delete itr.second;
        }
        for (int i = 0; i < mGame->playerNum(); ++i)
        {
            if (mUpdate[i])
            {
                continue;
            }
            for (auto &itr : mFixedStrategies[i])
            {
                delete itr.second;
            }
        }
        delete[] mFixedStrategies;
        delete[] mUpdate;
        delete mGame;
    }

    // @brief Calculates the payoff for each player in a given game state.
    // @param game The current state of the game.
    // @param strategies A vector of functions that return the strategy for each player.
    // @return A vector of payoffs for each player.
    template <typename Type>
    std::vector<double> Trainer<Type>::CalculatePayoff(const Type &game, const std::vector<std::function<const double *(const Type &)>> &strategies)
    {

        if (game.isGameOver())
        {
            std::vector<double> payoffs(game.playerNum());
            for (int i = 0; i < game.playerNum(); ++i)
            {
                payoffs[i] = game.payoff(i);
            }
            return payoffs;
        }

        const int actionNum = game.actionNum();
        if (game.isChanceNode())
        {
            std::vector<double> nodeUtils(game.playerNum());
            for (int i = 0; i < game.playerNum(); ++i)
            {
                nodeUtils[i] = 0.0;
            }
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const double chanceProbability = game_cp.chanceProbability();
                std::vector<double> utils = CalculatePayoff(game_cp, strategies);
                for (int i = 0; i < game.playerNum(); ++i)
                {
                    nodeUtils[i] += chanceProbability * utils[i];
                }
            }
            return nodeUtils;
        }

        const int player = game.currentPlayer();
        std::vector<double> nodeUtils(game.playerNum());
        for (int i = 0; i < game.playerNum(); ++i)
        {
            nodeUtils[i] = 0.0;
        }
        for (int a = 0; a < actionNum; ++a)
        {
            auto game_cp(game);
            game_cp.takeAction(a);
            std::vector<double> utils = CalculatePayoff(game_cp, strategies);
            for (int i = 0; i < game.playerNum(); ++i)
            {
                nodeUtils[i] += strategies[player](game)[a] * utils[i];
            }
        }
        return nodeUtils;
    }

    // @brief Calculates the exploitability of the current strategies in the game.
    // @param game The current state of the game.
    // @param strategies A vector of functions that return the strategy for each player.
    // @return The exploitability value.
    template <typename Type>
    double Trainer<Type>::CalculateExploitability(const Type &game, const std::vector<std::function<const double *(const Type &)>> &strategies)
    {
        InfoSets infoSets;
        for (int p = 0; p < game.playerNum(); ++p)
        {
            auto game_cp(game);
            game_cp.resetGame(false);
            CreateInfoSets(game_cp, p, strategies, 1.0, infoSets);
        }

        double exploitability = 0.0;
        for (int p = 0; p < game.playerNum(); ++p)
        {
            auto game_cp(game);
            game_cp.resetGame(false);
            std::unordered_map<std::string, std::vector<double>> bestResponseStrategies;
            exploitability += CalculateBestResponseValue(game_cp, p, strategies, bestResponseStrategies, 1.0, infoSets);
        }
        return exploitability;
    }

    // @brief Creates information sets for the game from the perspective of a specific player.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom information sets are being created.
    // @param strategies A vector of functions that return the strategy for each player.
    // @param po The probability of observing the current game state.
    // @param infoSets The map where information sets are stored.
    template <typename Type>
    void Trainer<Type>::CreateInfoSets(const Type &game, const int playerIndex, const std::vector<std::function<const double *(const Type &)>> &strategies, const double po, InfoSets &infoSets)
    {

        if (game.isGameOver())
        {
            return;
        }

        const int actionNum = game.actionNum();
        if (game.isChanceNode())
        {
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const double chanceProbability = game_cp.chanceProbability();
                CreateInfoSets(game_cp, playerIndex, strategies, po * chanceProbability, infoSets);
            }
            return;
        }

        const int player = game.currentPlayer();
        if (player == playerIndex)
        {
            std::string infoSet = game.infoSetStr();
            if (infoSets.count(infoSet) == 0)
            {
                infoSets[infoSet] = std::vector<std::tuple<Type, double>>();
            }
            infoSets[infoSet].push_back(std::make_tuple(game, po));
        }

        for (int a = 0; a < actionNum; ++a)
        {
            auto game_cp(game);
            game_cp.takeAction(a);
            if (player == playerIndex)
            {
                CreateInfoSets(game_cp, playerIndex, strategies, po, infoSets);
            }
            else
            {
                const double actionProb = strategies[player](game)[a];
                CreateInfoSets(game_cp, playerIndex, strategies, po * actionProb, infoSets);
            }
        }
    }

    // @brief Calculates the best response value for a player against the current strategies.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom the best response is being calculated.
    // @param strategies A vector of functions that return the strategy for each player.
    // @param bestResponseStrategies A map to store the best response strategies.
    // @param po The probability of observing the current game state.
    // @param infoSets The map of information sets.
    // @return The best response value for the player.
    template <typename Type>
    double Trainer<Type>::CalculateBestResponseValue(const Type &game, const int playerIndex,
                                                     const std::vector<std::function<const double *(const Type &)>> &strategies,
                                                     std::unordered_map<std::string, std::vector<double>> &bestResponseStrategies,
                                                     const double po,
                                                     const InfoSets &infoSets)
    {

        if (game.isGameOver())
        {
            return game.payoff(playerIndex);
        }

        const int actionNum = game.actionNum();
        if (game.isChanceNode())
        {
            double nodeUtil = 0.0;
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const double chanceProbability = game_cp.chanceProbability();
                nodeUtil += chanceProbability * CalculateBestResponseValue(game_cp, playerIndex, strategies, bestResponseStrategies, po * chanceProbability, infoSets);
            }
            return nodeUtil;
        }

        const int player = game.currentPlayer();
        if (player == playerIndex)
        {

            std::string infoSet = game.infoSetStr();
            if (bestResponseStrategies.count(infoSet) == 0)
            {

                double actionValues[actionNum];
                for (int a = 0; a < actionNum; ++a)
                {
                    actionValues[a] = 0.0;
                }
                for (int i = 0; i < infoSets.at(infoSet).size(); ++i)
                {
                    auto game_ = std::get<0>(infoSets.at(infoSet)[i]);
                    auto po_ = std::get<1>(infoSets.at(infoSet)[i]);
                    double brValues[actionNum];
                    for (int a = 0; a < actionNum; ++a)
                    {
                        auto game_cp(game_);
                        game_cp.takeAction(a);
                        brValues[a] = CalculateBestResponseValue(game_cp, playerIndex, strategies, bestResponseStrategies, po_, infoSets);
                        actionValues[a] += po_ * brValues[a];
                    }
                }

                int brAction = 0;
                for (int a = 0; a < actionNum; ++a)
                {
                    if (actionValues[a] > actionValues[brAction])
                    {
                        brAction = a;
                    }
                }
                bestResponseStrategies[infoSet] = std::vector<double>(actionNum, 0.0);
                bestResponseStrategies[infoSet][brAction] = 1.0;
            }

            double utils[actionNum];
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                utils[a] = CalculateBestResponseValue(game_cp, playerIndex, strategies, bestResponseStrategies, po, infoSets);
            }
            double bestResponseValue = 0.0;
            for (int a = 0; a < actionNum; ++a)
            {
                bestResponseValue += utils[a] * bestResponseStrategies.at(infoSet)[a];
            }
            return bestResponseValue;
        }
        else
        {

            double nodeUtil = 0.0;
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const double actionProb = strategies[player](game)[a];
                nodeUtil += actionProb * CalculateBestResponseValue(game_cp, playerIndex, strategies, bestResponseStrategies, po * actionProb, infoSets);
            }
            return nodeUtil;
        }
    }

    // @brief Trains the strategies using CFR for a specified number of iterations.
    // @param iterations The number of iterations to run the CFR algorithm.
    template <typename Type>
    void Trainer<Type>::train(const int iterations)
    {
        double utils[mGame->playerNum()];

        for (int i = 0; i < iterations; ++i)
        {
            for (int p = 0; p < mGame->playerNum(); ++p)
            {
                if (!mUpdate[p])
                {
                    continue;
                }
                if (mModeStr == "standard")
                {
                    mGame->resetGame(false);
                    utils[p] = CFR(*mGame, p, 1.0, 1.0);
                    for (auto &itr : mNodeMap)
                    {
                        itr.second->updateStrategy();
                    }
                }
                else
                {
                    mGame->resetGame();
                    if (mModeStr == "chance")
                    {
                        utils[p] = chanceSamplingCFR(*mGame, p, 1.0, 1.0);
                        for (auto &itr : mNodeMap)
                        {
                            itr.second->updateStrategy();
                        }
                    }
                    else if (mModeStr == "external")
                    {
                        utils[p] = externalSamplingCFR(*mGame, p);
                    }
                    else if (mModeStr == "outcome")
                    {
                        utils[p] = std::get<0>(outcomeSamplingCFR(*mGame, p, i, 1.0, 1.0, 1.0));
                    }
                    else
                    {
                        assert(false);
                    }
                }
            }
            if (i % 1000 == 0)
            {
                std::cout << "iteration:" << i << ", cumulative nodes touched: " << mNodeTouchedCnt << ", infosets num: " << mNodeMap.size() << ", expected payoffs: (";
                for (int p = 0; p < mGame->playerNum(); ++p)
                {
                    std::cout << utils[p] << ",";
                }
                std::cout << ")" << std::endl;
            }
            if (i != 0 && i % 10000000 == 0)
            {
                writeStrategyToBin(i);
            }
        }

        writeStrategyToBin();
    }

    // @brief Performs the standard CFR algorithm.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom CFR is being performed.
    // @param pi The product of the probabilities of actions taken by all players other than the current player.
    // @param po The product of the probabilities of actions taken by all players.
    // @return The utility value from the current game state.
    template <typename Type>
    double Trainer<Type>::CFR(const Type &game, const int playerIndex, const double pi, const double po)
    {
        ++mNodeTouchedCnt;

        if (game.isGameOver())
        {
            return game.payoff(playerIndex);
        }

        const int actionNum = game.actionNum();
        if (game.isChanceNode())
        {
            double nodeUtil = 0.0;
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const double chanceProbability = game_cp.chanceProbability();
                nodeUtil += chanceProbability * CFR(game_cp, playerIndex, pi, po * chanceProbability);
            }
            return nodeUtil;
        }

        std::string infoSet = game.infoSetStr();

        const int player = game.currentPlayer();
        if (!mUpdate[player])
        {
            double nodeUtil = 0.0;
            for (int a = 0; a < actionNum; ++a)
            {
                auto game_cp(game);
                game_cp.takeAction(a);
                const auto chanceProbability = double(mFixedStrategies[player].at(infoSet)->averageStrategy()[a]);
                nodeUtil += chanceProbability * CFR(game_cp, playerIndex, pi, po * chanceProbability);
            }
            return nodeUtil;
        }

        Node *node = mNodeMap[infoSet];
        if (node == nullptr)
        {
            node = new Node(actionNum);
            mNodeMap[infoSet] = node;
        }

        const double *strategy = node->strategy();

        double utils[actionNum];
        double nodeUtil = 0;
        for (int a = 0; a < actionNum; ++a)
        {
            auto game_cp(game);
            game_cp.takeAction(a);
            if (player == playerIndex)
            {
                utils[a] = CFR(game_cp, playerIndex, pi * strategy[a], po);
            }
            else
            {
                utils[a] = CFR(game_cp, playerIndex, pi, po * strategy[a]);
            }
            nodeUtil += strategy[a] * utils[a];
        }

        if (player == playerIndex)
        {

            for (int a = 0; a < actionNum; ++a)
            {
                const double regret = utils[a] - nodeUtil;
                const double regretSum = node->regretSum(a) + po * regret;
                node->regretSum(a, regretSum);
            }

            node->strategySum(strategy, pi);
        }

        return nodeUtil;
    }

    // @brief Performs the chance-sampling variant of CFR.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom CFR is being performed.
    // @param pi The product of the probabilities of actions taken by all players other than the current player.
    // @param po The product of the probabilities of actions taken by all players.
    // @return The utility value from the current game state.
    template <typename Type>
    double Trainer<Type>::chanceSamplingCFR(const Type &game, const int playerIndex, const double pi, const double po)
    {
        ++mNodeTouchedCnt;

        if (game.isGameOver())
        {
            return game.payoff(playerIndex);
        }

        std::string infoSet = game.infoSetStr();

        const int actionNum = game.actionNum();
        const int player = game.currentPlayer();
        if (!mUpdate[player])
        {
            auto game_cp(game);
            auto strategy = mFixedStrategies[player].at(infoSet)->averageStrategy();
            std::discrete_distribution<int> dist(strategy, strategy + actionNum);
            game_cp.takeAction(dist(randomGenerator));
            return chanceSamplingCFR(game_cp, playerIndex, pi, po);
        }

        Node *node = mNodeMap[infoSet];
        if (node == nullptr)
        {
            node = new Node(actionNum);
            mNodeMap[infoSet] = node;
        }

        const double *strategy = node->strategy();

        double utils[actionNum];
        double nodeUtil = 0;
        for (int a = 0; a < actionNum; ++a)
        {
            auto game_cp(game);
            game_cp.takeAction(a);
            if (player == playerIndex)
            {
                utils[a] = chanceSamplingCFR(game_cp, playerIndex, pi * strategy[a], po);
            }
            else
            {
                utils[a] = chanceSamplingCFR(game_cp, playerIndex, pi, po * strategy[a]);
            }
            nodeUtil += strategy[a] * utils[a];
        }

        if (player == playerIndex)
        {

            for (int a = 0; a < actionNum; ++a)
            {
                const double regret = utils[a] - nodeUtil;
                const double regretSum = node->regretSum(a) + po * regret;
                node->regretSum(a, regretSum);
            }

            node->strategySum(strategy, pi);
        }

        return nodeUtil;
    }

    // @brief Performs the external-sampling variant of CFR.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom CFR is being performed.
    // @return The utility value from the current game state.
    template <typename Type>
    double Trainer<Type>::externalSamplingCFR(const Type &game, const int playerIndex)
    {
        ++mNodeTouchedCnt;

        if (game.isGameOver())
        {
            return game.payoff(playerIndex);
        }

        std::string infoSet = game.infoSetStr();

        const int actionNum = game.actionNum();
        const int player = game.currentPlayer();
        assert(mUpdate[player] && "External sampling with stochastically-weighted averaging cannot treat static player.");

        Node *node = mNodeMap[infoSet];
        if (node == nullptr)
        {
            node = new Node(actionNum);
            mNodeMap[infoSet] = node;
        }

        node->updateStrategy();
        const double *strategy = node->strategy();

        if (player != playerIndex)
        {
            auto game_cp(game);
            std::discrete_distribution<int> dist(strategy, strategy + actionNum);
            game_cp.takeAction(dist(randomGenerator));
            const double util = externalSamplingCFR(game_cp, playerIndex);

            node->strategySum(strategy, 1.0);
            return util;
        }

        double utils[actionNum];
        double nodeUtil = 0;
        for (int a = 0; a < actionNum; ++a)
        {
            auto game_cp(game);
            game_cp.takeAction(a);
            utils[a] = externalSamplingCFR(game_cp, playerIndex);
            nodeUtil += strategy[a] * utils[a];
        }

        for (int a = 0; a < actionNum; ++a)
        {
            const double regret = utils[a] - nodeUtil;
            const double regretSum = node->regretSum(a) + regret;
            node->regretSum(a, regretSum);
        }

        return nodeUtil;
    }

    // @brief Performs the outcome-sampling variant of CFR.
    // @param game The current state of the game.
    // @param playerIndex The index of the player for whom CFR is being performed.
    // @param iteration The current iteration number.
    // @param pi The product of the probabilities of actions taken by all players other than the current player.
    // @param po The product of the probabilities of actions taken by all players.
    // @param s A scaling factor used in the sampling process.
    // @return A tuple containing the utility value and a probability factor.
    template <typename Type>
    std::tuple<double, double> Trainer<Type>::outcomeSamplingCFR(const Type &game, const int playerIndex, const int iteration, const double pi, const double po, const double s)
    {
        ++mNodeTouchedCnt;

        if (game.isGameOver())
        {
            return std::make_tuple(game.payoff(playerIndex) / s, 1.0);
        }

        std::string infoSet = game.infoSetStr();

        const int actionNum = game.actionNum();
        const int player = game.currentPlayer();
        assert(mUpdate[player] && "Outcome sampling with stochastically-weighted averaging cannot treat static player.");

        Node *node = mNodeMap[infoSet];
        if (node == nullptr)
        {
            node = new Node(actionNum);
            mNodeMap[infoSet] = node;
        }

        node->updateStrategy();
        const double *strategy = node->strategy();

        const double epsilon = 0.6;
        double probability[actionNum];
        if (player == playerIndex)
        {
            for (int a = 0; a < actionNum; ++a)
            {
                probability[a] = (epsilon / (double)actionNum) + (1.0 - epsilon) * strategy[a];
            }
        }
        else
        {
            for (int a = 0; a < actionNum; ++a)
            {
                probability[a] = strategy[a];
            }
        }
        std::discrete_distribution<int> dist(probability, probability + actionNum);
        const int chooseAction = dist(randomGenerator);

        double util, pTail;
        auto game_cp(game);
        game_cp.takeAction(chooseAction);
        const double newPi = pi * (player == playerIndex ? strategy[chooseAction] : 1.0);
        const double newPo = po * (player == playerIndex ? 1.0 : strategy[chooseAction]);
        std::tuple<double, double> ret = outcomeSamplingCFR(game_cp, playerIndex, iteration, newPi, newPo, s * probability[chooseAction]);
        util = std::get<0>(ret);
        pTail = std::get<1>(ret);
        if (player == playerIndex)
        {

            const double W = util * po;
            for (int a = 0; a < actionNum; ++a)
            {
                const double regret = a == chooseAction ? W * (1.0 - strategy[chooseAction]) * pTail : -W * pTail * strategy[chooseAction];
                const double regretSum = node->regretSum(a) + regret;
                node->regretSum(a, regretSum);
            }
        }
        else
        {

            node->strategySum(strategy, po / s);
        }
        return std::make_tuple(util, pTail * strategy[chooseAction]);
    }

    // @brief Writes the current strategies to a binary file.
    // @param iteration The iteration number to include in the file name (optional).
    template <typename Type>
    void Trainer<Type>::writeStrategyToBin(const int iteration) const
    {
        for (auto &itr : mNodeMap)
        {
            for (char c : itr.first)
            {
                std::cout << int(c);
            }
            std::cout << ":";
            for (int i = 0; i < itr.second->actionNum(); ++i)
            {
                std::cout << itr.second->averageStrategy()[i] << ",";
            }
            std::cout << std::endl;
        }
        std::string path = iteration > 0 ? "strategy_" + std::to_string(iteration)
                                         : "strategy";
        path += "_" + mModeStr + ".bin";
        std::ofstream ofs(mFolderPath + "/" + path);
        boost::archive::binary_oarchive oa(ofs);
        oa << mNodeMap;
        ofs.close();
    }

}