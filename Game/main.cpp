#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "cmdline.h"
#include "CFRAgent.hpp"
#include "CFRAgent.cpp"
#include "Game.hpp"
#include "Trainer.hpp"
#include "Trainer.cpp"

// defines the game
#define GAME Kuhn::Game

// main function
int main(int argc, char *argv[])
{
    // parse arguments
    cmdline::parser p;                                                                          // Create a command-line parser object
    p.add<uint32_t>("seed", 's', "Random seed used to initialize the random generator", false); // Add an optional argument for the random seed
    for (int i = 0; i < GAME::playerNum(); ++i)
    {
        p.add<std::string>("strategy-path-" + std::to_string(i), 0,
                           "Path to the binary file that represents the average strategy for player " + std::to_string(i), true); // Add arguments for each player's strategy file path
    }
    p.parse_check(argc, argv); // Parse and check the command-line arguments

    // create game
    std::mt19937 engine(p.exist("seed") ? p.get<uint32_t>("seed") : std::random_device()()); // Initialize the random generator with the provided seed or a random seed
    GAME game(engine);                                                                       // Create an instance of the game

    // initialize strategies
    std::vector<Agent::CFRAgent<GAME> *> cfrAgents(GAME::playerNum());                      // Vector to hold CFR agents for each player
    std::vector<std::function<const double *(const GAME &)>> strategies(GAME::playerNum()); // Vector to hold strategy functions for each player
    for (int i = 0; i < GAME::playerNum(); ++i)
    {
        cfrAgents[i] = new Agent::CFRAgent<GAME>(engine, p.get<std::string>("strategy-path-" + std::to_string(i))); // Initialize each CFR agent with the strategy file
        const Agent::CFRAgent<GAME> &agent = *cfrAgents[i];
        strategies[i] = [&agent](const GAME &game)
        { return agent.strategy(game); }; // Store the strategy function for each player
    }

    // calculate expected payoffs
    game.resetGame(false);                                                                   // Reset the game state
    std::vector<double> payoffs = Trainer::Trainer<GAME>::CalculatePayoff(game, strategies); // Calculate expected payoffs for the given strategies
    std::cout << "expected player payoffs: (";                                               // Output the expected payoffs
    for (int i = 0; i < GAME::playerNum(); ++i)
    {
        std::cout << payoffs[i] << ","; // Print the payoff for each player
    }
    std::cout << ")" << std::endl;

    // calculate exploitability
    game.resetGame(false);                                                                     // Reset the game state
    double exploitability = Trainer::Trainer<GAME>::CalculateExploitability(game, strategies); // Calculate the exploitability of the given strategies
    std::cout << "strategy exploitability: " << exploitability << std::endl;                   // Output the exploitability

    // finalize
    for (int i = 0; i < GAME::playerNum(); ++i)
    {
        delete cfrAgents[i]; // Clean up and delete the CFR agents
    }
}