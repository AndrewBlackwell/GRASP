Game-Theoretic Regret Analysis and Strategy Prediction built to help explore games with imperfect information and study how players can find optimal strategies.

The framework can handle complex game models and is useful for anyone interested in algorithmic game theory, decision-making, or reinforcement learning. **Kuhn Poker** is included as a starting example, but can work with much more complex games too.

## Acknowledgements

This project uses the **cmdline.h** library for command-line parsing. It's a header-only library that makes command-line interaction simple.

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

You can find more info on the [cmdline.h GitHub repository](https://github.com/tanakh/cmdline).

### References

Some papers and resources that helped with this project:

- **Harold W. Kuhn** (2007), "Preface to _The Essential John Nash_," Princeton University Press.
- **Todd W. Neller and Marc Lanctot** (2013), "An Introduction to Counterfactual Regret Minimization."
- **Martin Zinkevich et al.** (2007), "Regret Minimization in Games with Incomplete Information."
- **Michael Bowling et al.** (2015), "Heads-Up Limit Hold'em Poker is Solved."
- **Gabriele Farina et al.** (2017), "Regret Minimization in Behaviorally-Constrained Zero-Sum Games" presented at ICML.
- **Heinrich & Silver** (2016), "Neural Fictitious Self-Play (NFSP)" and "Deep Counterfactual Regret Minimization" (2018)

## Status Note

> _The Monte Carlo CFR variants (Chance-Sampled, External-Sampled, Outcome-Sampled) are still being worked on. Vanilla CFR is stable and works well, but the Monte Carlo methods might be buggy in some cases._
