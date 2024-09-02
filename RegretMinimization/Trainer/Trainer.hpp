#ifndef REGRETMINIMIZATION_TRAINER_HPP
#define REGRETMINIMIZATION_TRAINER_HPP

#include <functional>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Trainer
{
    class Node;
}

namespace Trainer
{
    // @brief The Trainer class template implements the Counterfactual Regret Minimization (CFR) training process.
    // @tparam Type The type of game being trained.
    template <typename Type>
    class Trainer
    {
    public:
        // @brief Defines a map that associates information sets with game states and their probabilities.
        using InfoSets = typename std::unordered_map<std::string, std::vector<std::tuple<Type, double>>>;

        // @brief Constructs a Trainer object with the specified mode, random seed, and strategy paths.
        // @param mode The mode of CFR to use (e.g., standard, chance, external, outcome).
        // @param seed A seed for the random number generator.
        // @param strategyPaths Optional paths to pre-existing strategies for players.
        explicit Trainer(const std::string &mode, uint32_t seed, const std::vector<std::string> &strategyPaths = {});

        // @brief Destructor for Trainer, responsible for cleaning up dynamically allocated memory.
        ~Trainer();

        // @brief Calculates the payoff for each player in a given game state.
        // @param game The current state of the game.
        // @param strategies A vector of functions that return the strategy for each player.
        // @return A vector of payoffs for each player.
        static std::vector<double> CalculatePayoff(const Type &game, const std::vector<std::function<const double *(const Type &)>> &strategies);

        // @brief Calculates the exploitability of the current strategies in the game.
        // @param game The current state of the game.
        // @param strategies A vector of functions that return the strategy for each player.
        // @return The exploitability value.
        static double CalculateExploitability(const Type &game, const std::vector<std::function<const double *(const Type &)>> &strategies);

        // @brief Creates information sets for the game from the perspective of a specific player.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom information sets are being created.
        // @param strategies A vector of functions that return the strategy for each player.
        // @param po The probability of observing the current game state.
        // @param infoSets The map where information sets are stored.
        static void CreateInfoSets(const Type &game, int playerIndex, const std::vector<std::function<const double *(const Type &)>> &strategies, double po, InfoSets &infoSets);

        // @brief Calculates the best response value for a player against the current strategies.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom the best response is being calculated.
        // @param strategies A vector of functions that return the strategy for each player.
        // @param bestResponseStrategies A map to store the best response strategies.
        // @param po The probability of observing the current game state.
        // @param infoSets The map of information sets.
        // @return The best response value for the player.
        static double CalculateBestResponseValue(const Type &game, int playerIndex, const std::vector<std::function<const double *(const Type &)>> &strategies, std::unordered_map<std::string, std::vector<double>> &bestResponseStrategies, double po, const InfoSets &infoSets);

        // @brief Trains the strategies using CFR for a specified number of iterations.
        // @param iterations The number of iterations to run the CFR algorithm.
        void train(int iterations);

    private:
        // @brief Performs the standard CFR algorithm.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom CFR is being performed.
        // @param pi The product of the probabilities of actions taken by all players other than the current player.
        // @param po The product of the probabilities of actions taken by all players.
        // @return The utility value from the current game state.
        double CFR(const Type &game, int playerIndex, double pi, double po);

        // @brief Performs the chance-sampling variant of CFR.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom CFR is being performed.
        // @param pi The product of the probabilities of actions taken by all players other than the current player.
        // @param po The product of the probabilities of actions taken by all players.
        // @return The utility value from the current game state.
        double chanceSamplingCFR(const Type &game, int playerIndex, double pi, double po);

        // @brief Performs the external-sampling variant of CFR.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom CFR is being performed.
        // @return The utility value from the current game state.
        double externalSamplingCFR(const Type &game, int playerIndex);

        // @brief Performs the outcome-sampling variant of CFR.
        // @param game The current state of the game.
        // @param playerIndex The index of the player for whom CFR is being performed.
        // @param iteration The current iteration number.
        // @param pi The product of the probabilities of actions taken by all players other than the current player.
        // @param po The product of the probabilities of actions taken by all players.
        // @param s A scaling factor used in the sampling process.
        // @return A tuple containing the utility value and a probability factor.
        std::tuple<double, double> outcomeSamplingCFR(const Type &game, int playerIndex, int iteration, double pi, double po, double s);

        // @brief Writes the current strategies to a binary file.
        // @param iteration The iteration number to include in the file name (optional).
        void writeStrategyToBin(int iteration = -1) const;

        std::mt19937 randomGenerator;                              // Random number generator for sampling actions.
        std::unordered_map<std::string, Node *> mNodeMap;          // Map of information sets to nodes containing strategies and regrets.
        uint64_t mNodeTouchedCnt;                                  // Counter for the number of nodes touched during training.
        Type *mGame;                                               // Pointer to the game being trained.
        std::string mFolderPath;                                   // Path to the folder where strategies are saved.
        const std::string &mModeStr;                               // Mode string indicating the variant of CFR being used.
        std::unordered_map<std::string, Node *> *mFixedStrategies; // Array of maps for fixed strategies, one for each player.
        bool *mUpdate;                                             // Array indicating which players' strategies are being updated.
    };

}

#endif