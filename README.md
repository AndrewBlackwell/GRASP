# Argonaut: Counterfactual Regret Minimization Engine

!["Argonaut Logo"](/docs/images/Argonaut.jpg)

###### _Argonaut - noun. Classical Mythology. a member of the band of men who sailed to Colchis with Jason in the ship Argo in search of the Golden Fleece. (sometimes lowercase) a person in quest of something dangerous but rewarding; adventurer._

### Argonaut is a C++ implementation of various Counterfactual Regret Minimization (CFR) algorithms for approximating Nash equilibria in extensive-form games with imperfect information. This particular project focuses on Kuhn Poker, a two-player zero-sum variant of Poker with a considerably more manageable decision-tree. This makes it an ideal testbed for developing and testing CFR algorithms as a means for learning introductory reinforcement learning concepts.

The following algorithms are available for use:

- **Standard (standard) CFR**

  This is the basic form of CFR, where the algorithm iteratively updates regrets for not taking certain actions at each decision point in a game. Over many iterations, it adjusts strategies based on these regrets, gradually converging towards a Nash equilibrium.

- **Chance-Sampled Monte Carlo CFR**

  In games with elements of chance (like card games), this variant samples possible chance events (like card deals) and only updates regrets for the sampled outcomes. This reduces the computational complexity compared to updating regrets for all possible outcomes.

- **External-Sampled Monte Carlo CFR**

  Similar to chance-sampled, but it focuses on sampling the opponent’s actions (external actions). It updates regrets based on these sampled actions, which helps in efficiently approximating the best response strategies without needing to consider every possible action the opponent might take.

- **Outcome-Sampled Monte Carlo CFR**

  This variant goes even further by sampling entire outcomes of the game, updating regrets only for the actions that actually occurred in the sampled outcome. This can be much faster but may require more iterations to converge since it only considers a small subset of possible game trajectories each time.

## Dependencies

1. **C++ Compiler Supporting C++14**:

   - **Why**: C++14 introduces enhancements and features that improve the development process and code efficiency, which this project takes advantage of.

2. **[CMake](https://cmake.org/) 3.5 or Higher**:

   - **Why**: CMake simplifies the process of configuring and building projects across different environments. It generates platform-specific build files, making it easier to compile and link the project on any operating system.

3. **[Boost](https://www.boost.org/) 1.68.0 or Higher**:
   - **Why**: This project relies on boost for crucial algorithms. Boost is known for its robustness and is widely regarded as an extension of the standard C++ library.

## Running Simulations

### Mersenne Twister Algorithm for Pseudo-Random Number Generation

This project employs the Mersenne Twister algorithm for pseudorandom number generation (PRNG). It is included in the C++ standard libary as `std::mt19937` The Mersenne Twister is a widely-used PRNG that generates high-quality pseudorandom numbers. It was developed by Makoto Matsumoto and Takuji Nishimura in 1997. The name “Mersenne" comes from the fact that it has a period of `2^19937−1`, which is a Mersenne prime. The “Twister” in Mersenne Twister comes from the algorithm’s process of maintaining a state vector of 624 integers. During each iteration, this state vector is updated using bitwise operations to “twist” the bits, ensuring that the resulting pseudorandom numbers have excellent statistical properties. Specifically, the algorithm applies a combination of shifts, masks, and XOR operations to mix the bits thoroughly, which helps produce a high-quality and long-period sequence of random numbers.

**Specifically, the Mersenne Twister is used in this project for the following reasons:**

- The Mersenne Twister has an extremely long period of 2^19937−1, meaning it can generate a vast sequence of random numbers before repeating. This is particularly useful in simulations and algorithms like CFR, where a large number of random samples are needed.
- The algorithm is known for its high quality of randomness, passing numerous statistical tests for randomness. This ensures that the outcomes in each iteration (or hand of Kuhn Poker) are as unpredictable as possible, which is crucial for accurately simulating game scenarios.

### Building the Project

First, you’ll need to build the project. Run the following commands in your terminal to build the engine:

```bash
$ sh build.sh
$ cd build
```

### Running an Experiment

To compute an approximate Nash equilibrium using standard CFR on 1,000,000 hands of Kuhn Poker, execute the following command:

```bash
$ ./RegretMinimization/run_cfr --iterations=1000000
```

Additional options include:

- `--algorithm`: Specify the CFR algorithm to use (`"standard"`, `"chance"`, `"external"`, or `"outcome"`). Default is `"standard"`.
- `--iterations`: Number of CFR iterations to run.
- `--seed`: Seed for the random number generator. If omitted, it defaults to a seed from `std::random_device`.

After the experiment completes, the resulting average strategy is saved as a binary file, such as `strategies/kuhn/strategy_standard.bin`.

### Evaluating Strategies

Once you've computed an approximate Nash equilibrium, you can evaluate the strategies by calculating expected payoffs and exploitability:

```bash
$ ./Game/game --strategy-path-0="../strategies/kuhn/strategy_standard.bin" --strategy-path-1="../strategies/kuhn/strategy_standard.bin"
```

This command requires paths to the binary files containing the strategies for each player, such as `strategy-path-0` and `strategy-path-1`. As such, you can compare the two. The results will include the expected payoffs and exploitability for the specified strategies:

```bash
expected player payoffs: (-0.0555556, 0.0555556)
strategy exploitability: 6.59955e-06
```

The payoffs represent the expected outcome for each player when using the given strategies. If you see something like (-0.0555556, 0.0555556), it indicates that Player 1 is expected to lose about 0.0556 units per game, while Player 2 is expected to gain the same amount. These values help you understand the relative strength of the strategies being used by each player. Exploitability measures how far a strategy is from being optimal or unbeatable (a Nash equilibrium). If the exploitability is close to zero, like 6.59955e-06, it means the strategy is nearly optimal, and an opponent can’t gain much by deviating from their own strategy. Higher exploitability suggests that there are weaknesses in the strategy that a savvy opponent could exploit to increase their own payoff.

In essence, low exploitability and balanced payoffs suggest strong, competitive strategies, while higher exploitability or skewed payoffs might indicate that one player’s strategy is more vulnerable or less effective.

## Docker Instructions

This repository includes a `Dockerfile` for easy setup with Docker. Build and run the container using:

```bash
$ docker build -t argonaut .
$ docker run -it argonaut
```

## References

- **Harold W. Kuhn** (2007). "Preface to _The Essential John Nash_," Introductory Chapters, in: Harold W. Kuhn & Sylvia Nasar (ed.), _The Essential John Nash_, Princeton University Press. This work provided foundational insights into game theory, particularly Nash equilibrium, which is central to the algorithms implemented.

- **Todd W. Neller and Marc Lanctot** (2013). _"An Introduction to Counterfactual Regret Minimization," July 9, 2013_. This paper was instrumental in understanding and implementing the various CFR algorithms.

These contributions have been invaluable in the development and refinement of Argonaut, and we extend our sincere thanks to the authors and developers for their work.
