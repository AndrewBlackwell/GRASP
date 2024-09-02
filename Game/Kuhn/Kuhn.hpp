#ifndef GAME_KUHN_HPP
#define GAME_KUHN_HPP

namespace Kuhn
{

    // @enum Action
    // @brief Represents the possible actions a player can take in Kuhn Poker.
    enum class Action : int
    {
        NONE = -1, // Indicates no chooseAction has been taken or an invalid state.
        PASS = 0,  // The player chooses to pass.
        BET,       // The player chooses to bet or call.
        NUM        // The total number of actions (used for iteration or boundary checks).
    };
}

#endif