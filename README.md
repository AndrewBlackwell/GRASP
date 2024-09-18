# GRASP: Game-Theoretic Regret Analysis and Strategy Prediction

!["GRASP Branding"](/docs/images/grasp1.jpg)

#### GRASP is an advanced C++ framework for implementing Counterfactual Regret Minimization (CFR) algorithms. It is designed to facilitate the exploration and analysis of extensive-form games with imperfect information, offering tools for studying equilibrium strategies and dynamic behavior in competitive environments.

The framework is equipped to handle complex game-theoretic models and is intended for researchers and students with expertise in *(or willingless to learn)* algorithmic game theory, decision-making processes, and reinforcement learning. While its architecture is flexible, **Kuhn Poker** is provided solely as an example of an initial application. GRASP is capable of accommodating more complex and diverse game structures beyond this introductory use case.

### Core Capabilities

GRASP integrates various advanced CFR algorithms to approximate Nash equilibria and analyze strategic behavior in games with imperfect information:

- **Standard CFR**: Iteratively updates regrets and strategies, converging towards equilibrium through regret minimization.
- **Monte Carlo Variants**: Includes Chance-Sampled, External-Sampled, and Outcome-Sampled methods, designed to reduce computational complexity while maintaining robustness in large decision trees.

The algorithms implemented within GRASP are based on current research in computational game theory and are suitable for studying optimal play in strategic decision-making scenarios, particularly those involving hidden information or stochastic elements.

### Example Use Case: Kuhn Poker

While GRASP is built to support general extensive-form games, Kuhn Poker—a simplified two-player, zero-sum poker variant—has been included as an illustrative example. This serves as a manageable decision-tree model for the study of CFR algorithms.

## Acknowledgements

GRASP utilizes the **cmdline.h** library for parsing command-line input. This header-only library provides an efficient and flexible interface for command-line interaction with minimal overhead.

The **cmdline.h** library is available under the following license:

```text
Copyright (c) 2009, Hideyuki Tanaka

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions, and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions, and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of Hideyuki Tanaka nor the names of other contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
```

You can find more information and the full source code on the official [cmdline.h GitHub repository](https://github.com/tanakh/cmdline).

### Reference Materials

For further theoretical grounding and algorithmic details, GRASP draws on foundational work in game theory and regret minimization:

- **Harold W. Kuhn** (2007), "Preface to _The Essential John Nash_," Princeton University Press.
- **Todd W. Neller and Marc Lanctot** (2013), "An Introduction to Counterfactual Regret Minimization."
- **Martin Zinkevich et al.** (2007), "Regret Minimization in Games with Incomplete Information."
- **Michael Bowling et al.** (2015), "Heads-Up Limit Hold'em Poker is Solved."
- **Gabriele Farina et al.** (2017), “Regret Minimization in Behaviorally-Constrained Zero-Sum Games” presented at ICML.
- **Heinrich & Silver** (2016), "Neural Fictitious Self-Play (NFSP)" and "Deep Counterfactual Regret Minimization" (2018)

## DEVELOPMENT UPDATES

> _The Monte Carlo CFR variants (Chance-Sampled, External-Sampled, Outcome-Sampled) are currently under refinement. While the vanilla CFR implementation is fully functional, these methods are still experimental and may exhibit instability in certain edge cases._
