#ifndef GAME_CONSTANT_HPP
#define GAME_CONSTANT_HPP

namespace Kuhn
{

    /// @brief Number of players in the game.
    static const int numPlayers = 2;

    /// @brief Number of cards in the game, which is one more than the number of players.
    static const int numCards = numPlayers + 1;

    /// @brief Calculates the number of possible chance actions (factorial of numCards).
    /// @return The factorial of numCards, representing the total number of permutations of the cards.
    static constexpr int ChanceActionNum()
    {
        int actionNum = 1;
        for (int i = 1; i <= numCards; i++)
        {
            actionNum *= i;
        }
        return actionNum;
    }
}

#endif