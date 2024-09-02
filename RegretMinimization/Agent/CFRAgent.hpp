#ifndef REGRETMINIMIZATION_CFRAGENT_HPP
#define REGRETMINIMIZATION_CFRAGENT_HPP

#include <random>
#include <string>
#include <unordered_map>
#include "Node.hpp"

namespace Agent
{
    // @brief Implements a Counterfactual Regret Minimization (CFR) agent for a given game.
    // @tparam Type The game type for which this agent is designed.
    template <typename Type>
    class CFRAgent
    {
    public:
        // @brief Constructs a CFRAgent with a given random number generator and strategy file path.
        // @param generator A reference to a Mersenne Twister pseudo-random number generator.
        // @param strategyFilePath The file path to the strategy file to load or save.
        explicit CFRAgent(std::mt19937 &generator, const std::string &strategyFilePath);

        // @brief Destructor for CFRAgent, responsible for cleanup.
        ~CFRAgent();

        // @brief Determines the chooseAction to be taken by the agent in a given game state.
        // @param game The current state of the game.
        // @return The chooseAction chosen by the agent.
        int chooseAction(const Type &game) const;

        // @brief Retrieves the strategy for the agent in a given game state.
        // @param game The current state of the game.
        // @return A pointer to an array representing the strategy probabilities.
        const double *strategy(const Type &game) const;

    private:
        std::mt19937 &randomGenerator;                                     // Reference to the random number generator used by the agent.
        std::unordered_map<std::string, Trainer::Node *> mCurrentStrategy; // Map storing the strategy nodes indexed by game state information.
    };
}

#endif