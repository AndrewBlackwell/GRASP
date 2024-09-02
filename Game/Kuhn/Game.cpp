#include "Game.hpp"
#include "Kuhn.hpp"

#include <algorithm>
#include <cstring>

namespace Kuhn
{

    // @brief Constructor initializing the game with a random number generator and setting initial values for game state variables.
    Game::Game(std::mt19937 &generator) : randomGenerator(generator), playerPayoff(), currentPlayerIndex(-1), chanceProb(0.0), firstBetTurnIndex(-1), playerBetNumber(0), turnIndex(0), gameOver(false)
    {
        // Initialize the information sets for each player
        for (auto &infoSet : mInfoSets)
        {
            for (uint8_t &i : infoSet)
            {
                i = 0;
            }
        }
    }

    // @brief Copy constructor creating a deep copy of an existing Game object.
    Game::Game(const Game &obj) : randomGenerator(obj.randomGenerator), playerCards(obj.playerCards), playerPayoff(obj.playerPayoff),
                                  currentPlayerIndex(obj.currentPlayerIndex), chanceProb(obj.chanceProb), firstBetTurnIndex(obj.firstBetTurnIndex), playerBetNumber(obj.playerBetNumber), turnIndex(obj.turnIndex), gameOver(obj.gameOver)
    {
        // Copy information sets from the source object
        for (int i = 0; i < numPlayers; ++i)
        {
            std::memcpy(mInfoSets[i], obj.mInfoSets[i], obj.turnIndex + 1);
        }
    }

    // @brief Returns the name of the game as a string.
    std::string Game::name()
    {
        return "kuhn";
    }

    // @brief Returns the number of players in the game.
    int Game::playerNum()
    {
        return numPlayers;
    }

    // @brief Resets the game state for a new round, optionally skipping the chance player's chooseAction.
    void Game::resetGame(bool skipChanceAction)
    {
        if (!skipChanceAction)
        {
            currentPlayerIndex = numPlayers + 1;
            return;
        }

        // Initialize the deck and shuffle the cards
        for (int i = 0; i < numCards; ++i)
        {
            playerCards[i] = i;
        }

        for (int c1 = int(playerCards.size()) - 1; c1 > 0; --c1)
        {
            const int c2 = int(randomGenerator()) % (c1 + 1);
            const int tmp = playerCards[c1];
            playerCards[c1] = playerCards[c2];
            playerCards[c2] = tmp;
        }

        // Assign initial information sets and reset game state variables
        for (int i = 0; i < numPlayers; ++i)
        {
            mInfoSets[i][0] = playerCards[i];
        }
        turnIndex = 0;
        currentPlayerIndex = 0;
        firstBetTurnIndex = -1;
        playerBetNumber = 0;
        gameOver = false;
    }

    // @brief Processes the chooseAction taken by the current player and updates the game state.
    void Game::takeAction(const int chooseAction)
    {

        // Handle actions for the chance player
        if (currentPlayerIndex == numPlayers + 1)
        {
            constexpr int ChanceAN = ChanceActionNum();
            chanceProb = 1.0 / double(ChanceAN);
            for (int i = 0; i < numCards; ++i)
            {
                playerCards[i] = i;
            }

            int a = chooseAction;
            for (int c1 = int(playerCards.size()) - 1; c1 > 0; --c1)
            {
                const int c2 = a % (c1 + 1);
                const int tmp = playerCards[c1];
                playerCards[c1] = playerCards[c2];
                playerCards[c2] = tmp;
                a = (int)a / (c1 + 1);
            }
            for (int i = 0; i < numPlayers; ++i)
            {
                mInfoSets[i][0] = playerCards[i];
            }
            turnIndex = 0;
            currentPlayerIndex = 0;
            firstBetTurnIndex = -1;
            playerBetNumber = 0;
            gameOver = false;
            return;
        }

        // Update the game state based on the chooseAction taken
        turnIndex += 1;
        playerBetNumber += chooseAction;
        for (auto &infoSet : mInfoSets)
        {
            infoSet[turnIndex] = chooseAction;
        }
        if (firstBetTurnIndex == -1 && chooseAction == 1)
        {
            firstBetTurnIndex = turnIndex;
        }

        const int plays = turnIndex;
        const int player = plays % numPlayers;
        int opponents[numPlayers - 1];
        for (int i = 0; i < numPlayers - 1; ++i)
        {
            opponents[i] = (player + i + 1) % numPlayers;
        }
        if (plays > 1)
        {
            // Determine if the game has reached a terminal pass state
            const bool terminalPass = (firstBetTurnIndex > 0 && (turnIndex - firstBetTurnIndex == numPlayers - 1)) || (turnIndex == numPlayers && firstBetTurnIndex == -1 && mInfoSets[0][turnIndex] == 0);

            // Handle the outcome of the game based on the actions taken
            if (playerBetNumber == numPlayers)
            {
                const size_t winPlayer = std::distance(playerCards.begin(), std::max_element(playerCards.begin(), playerCards.begin() + numPlayers));
                playerPayoff[winPlayer] = 2 * (numPlayers - 1);
                for (int i = 0; i < numPlayers; ++i)
                {
                    if (i == winPlayer)
                    {
                        continue;
                    }
                    playerPayoff[i] = -2;
                }
                gameOver = true;
            }
            else if (terminalPass)
            {

                if (playerBetNumber == 0)
                {
                    const size_t winPlayer = std::distance(playerCards.begin(), std::max_element(playerCards.begin(), playerCards.begin() + numPlayers));
                    playerPayoff[winPlayer] = numPlayers - 1;
                    for (int i = 0; i < numPlayers; ++i)
                    {
                        if (i == winPlayer)
                        {
                            continue;
                        }
                        playerPayoff[i] = -1;
                    }
                    gameOver = true;
                }

                else if (playerBetNumber == 1)
                {
                    playerPayoff[player] = numPlayers - 1;
                    for (int i = 0; i < numPlayers; ++i)
                    {
                        if (i == player)
                        {
                            continue;
                        }
                        playerPayoff[i] = -1;
                    }
                    gameOver = true;
                }

                else if (playerBetNumber >= 2)
                {
                    std::array<int, numPlayers> card{};
                    card.fill(-1);
                    std::array<bool, numPlayers> isBet{};
                    isBet.fill(false);
                    for (int i = 0; i < turnIndex; ++i)
                    {
                        if (mInfoSets[0][i + 1] == 1)
                        {
                            card[i % numPlayers] = playerCards[i % numPlayers];
                            isBet[i % numPlayers] = true;
                        }
                    }
                    const size_t winPlayer = std::distance(card.begin(), std::max_element(card.begin(), card.end()));
                    playerPayoff[winPlayer] = 2 * (playerBetNumber - 1) + (numPlayers - playerBetNumber);
                    for (int i = 0; i < numPlayers; ++i)
                    {
                        if (!isBet[i])
                        {
                            playerPayoff[i] = -1;
                            continue;
                        }
                        if (i == winPlayer)
                        {
                            continue;
                        }
                        playerPayoff[i] = -2;
                    }
                    gameOver = true;
                }
            }
        }
        currentPlayerIndex = player;
    }

    // @brief Returns the payoff for the specified player.
    double Game::payoff(const int playerIndex) const
    {
        return playerPayoff[playerIndex];
    }

    // @brief Returns a string representation of the current information set for the acting player.
    std::string Game::infoSetStr() const
    {
        return std::string((char *)mInfoSets[currentPlayerIndex], turnIndex + 1);
    }

    // @brief Checks if the game is over.
    bool Game::isGameOver() const
    {
        return gameOver;
    }

    // @brief Returns the number of available actions at the current game state.
    int Game::actionNum() const
    {
        if (currentPlayerIndex == numPlayers + 1)
        {
            constexpr int ChanceAN = ChanceActionNum();
            return ChanceAN;
        }
        return (int)Action::NUM;
    }

    // @brief Returns the index of the current acting player.
    int Game::currentPlayer() const
    {
        return currentPlayerIndex;
    }

    // @brief Returns the probability associated with the last chooseAction taken by the chance player.
    double Game::chanceProbability() const
    {
        return chanceProb;
    }

    // @brief Checks if the current player is the chance player.
    bool Game::isChanceNode() const
    {
        return currentPlayerIndex == numPlayers + 1;
    }
}