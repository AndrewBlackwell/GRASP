#include <random>
#include <string>
#include "cmdline.h"
#include "Game.hpp"
#include "Trainer.hpp"
#include "Trainer.cpp"

// @brief Main function to run the training of the CFR algorithm.
int main(int argc, char *argv[])
{
    // Create a command-line parser object
    cmdline::parser p;

    // Add a command-line argument to specify the CFR algorithm variant (default is "standard")
    p.add<std::string>("algorithm", 'a',
                       "A variant of CFR algorithm computing an equilibrium (default \"standard\")",
                       false, "standard",
                       cmdline::oneof<std::string>("standard", "chance", "external", "outcome"));

    // Add a command-line argument to specify the number of iterations for CFR
    p.add<uint64_t>("iteration", 'i', "Number of iterations of CFR", true);

    // Add a command-line argument to specify the random seed for initialization
    p.add<uint32_t>("seed", 's', "Random seed used to initialize the random generator", false);

    // Parse and check the command-line arguments
    p.parse_check(argc, argv);

    // Initialize the trainer with the specified algorithm and seed
    Trainer::Trainer<Kuhn::Game> trainer(p.get<std::string>("algorithm"),
                                         p.exist("seed") ? p.get<uint32_t>("seed") : std::random_device()());

    // Run the training for the specified number of iterations
    trainer.train(int(p.get<uint64_t>("iteration")));
}