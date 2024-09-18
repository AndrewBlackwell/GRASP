#include "CFRAgent.hpp"
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>

namespace Agent
{
    // @brief Constructs a CFRAgent object, loading the strategy map from a file.
    // @param engine A reference to a Mersenne Twister pseudo-random number generator.
    // @param path The file path to the strategy file to load.
    template <typename Type>
    CFRAgent<Type>::CFRAgent(std::mt19937 &engine, const std::string &path) : randomGenerator(engine)
    {
        std::ifstream ifs(path);                 // Open the strategy file for input
        boost::archive::binary_iarchive ia(ifs); // Create an input archive for deserialization
        ia >> mCurrentStrategy;                  // Load the strategy map from the file
        ifs.close();                             // Close the file stream
    }

    // @brief Destructor for CFRAgent, responsible for cleaning up dynamically allocated memory.
    template <typename Type>
    CFRAgent<Type>::~CFRAgent()
    {
        // Delete all dynamically allocated strategy nodes in the strategy map
        for (auto &itr : mCurrentStrategy)
        {
            delete itr.second;
        }
    }

    // @brief Determines the action to be taken by the agent in a given game state.
    // @param game The current state of the game.
    // @return The action chosen by the agent based on the current strategy.
    template <typename Type>
    int CFRAgent<Type>::chooseAction(const Type &game) const
    {
        // If there is only one possible action, return 0 (no choice needed)
        if (game.actionNum() == 1)
        {
            return 0;
        }

        // Retrieve the information set string representation for the current game state
        const std::string infoSetStr = game.infoSetStr();

        // Retrieve the average strategy for this information set
        const double *probability = mCurrentStrategy.at(infoSetStr)->averageStrategy();

        // Use a discrete distribution to randomly select an action based on the strategy probabilities
        std::discrete_distribution<int> dist(probability, probability + game.actionNum());
        return dist(randomGenerator);
    }

    // @brief Retrieves the strategy for the agent in a given game state.
    // @param game The current state of the game.
    // @return A pointer to an array representing the strategy probabilities.
    template <typename Type>
    const double *CFRAgent<Type>::strategy(const Type &game) const
    {
        // Retrieve the strategy probabilities for the current game state
        return mCurrentStrategy.at(game.infoSetStr())->averageStrategy();
    }
}