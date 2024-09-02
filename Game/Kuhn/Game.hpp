#ifndef GAME_GAME_HPP
#define GAME_GAME_HPP

#include <array>
#include <random>
#include <string>
#include "Constant.hpp"

namespace Kuhn
{

    // @class Game
    // @brief Manages the state and logic of a Kuhn Poker game.
    class Game
    {
    public:
        // @brief Constructs a Game object using the provided random number generator.
        // @param generator A reference to a Mersenne Twister pseudo-random number generator.
        explicit Game(std::mt19937 &generator);

        // @brief Copy constructor for creating a new Game object as a copy of an existing one.
        // @param obj The Game object to copy from.
        Game(const Game &obj);

        // @brief Returns the name of the game.
        // @return A string representing the game's name.
        static std::string name();

        // @brief Returns the number of players in the game.
        // @return The number of players as an integer.
        static int playerNum();

        // @brief Resets the game state, optionally skipping the chance player's chooseAction.
        // @param skipChanceAction If true, skips the chance player's chooseAction during reset.
        void resetGame(bool skipChanceAction = true);

        // @brief Advances the game state based on the given chooseAction.
        // @param chooseAction The chooseAction to be performed by the current player.
        void takeAction(int chooseAction);

        // @brief Retrieves the payoff for a specific player.
        // @param playerIndex The index of the player whose payoff is being requested.
        // @return The payoff for the specified player.
        double payoff(int playerIndex) const;

        // @brief Provides a string representation of the current information set.
        // @return A string representing the current information set.
        std::string infoSetStr() const;

        // @brief Checks if the game is over.
        // @return True if the game has ended, false otherwise.
        bool isGameOver() const;

        // @brief Returns the number of available actions for the current game state.
        // @return The number of available actions as an integer.
        int actionNum() const;

        // @brief Returns the index of the current acting player.
        // @return The index of the current player.
        int currentPlayer() const;

        // @brief Returns the probability associated with the last chooseAction of the chance player.
        // @return The probability of the last chooseAction as a double.
        double chanceProbability() const;

        // @brief Checks if the current player is the chance player.
        // @return True if the current player is the chance player, false otherwise.
        bool isChanceNode() const;

    private:
        std::mt19937 &randomGenerator;               // Random number generator reference used in the game.
        std::array<int, numCards> playerCards;       // Array holding the cards dealt to the players.
        std::array<double, numPlayers> playerPayoff; // Array storing the payoffs for each player.
        int currentPlayerIndex;                      // Index of the current player in the game.
        double chanceProb;                           // Probability of the last chooseAction performed by the chance player.
        int firstBetTurnIndex;                       // The turn index of the first betting chooseAction.
        int playerBetNumber;                         // Number of players who have placed a bet.
        int turnIndex;                               // Current turn index in the game.
        bool gameOver;                               // Flag indicating if the game has ended.
        uint8_t mInfoSets[numPlayers][10];           // Information sets for the players during the game.
    };

}

#endif