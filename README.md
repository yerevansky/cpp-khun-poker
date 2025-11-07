# CFR Kuhn Poker Solver (C++)

This repository contains an implementation of the **Counterfactual Regret Minimization (CFR)**
algorithm applied to the simplified poker game **Kuhn Poker**.

The algorithm converges to the Nash equilibrium and reconstructs optimal mixed strategies
for both players.

---

## ✅ About Kuhn Poker

Kuhn Poker is the simplest zero-sum imperfect-information poker game:

- Deck: `J`, `Q`, `K`
- Each player antes (1 chip)
- Each receives 1 card
- Allowed actions:
  - `c` → check
  - `b` → bet
- Bluffing is optimal — making it a popular benchmark for CFR

The theoretical expected value:

```

EV(Player 1) ≈ -0.055
EV(Player 2) ≈ +0.055

```

This implementation converges to that solution.

---

## ✅ Project Structure

```

/project-root
│
├── CMakeLists.txt
├── src/
│   └── *.cpp      # CFR implementation, main()
└── README.md

````

---

## ✅ Building

### Requirements

- CMake ≥ 3.14
- C++17 compiler (GCC/Clang/MSVC)

### Build (Linux / macOS)

```bash
mkdir build
cd build
cmake ..
cmake --build .
````

### Build (Windows, Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build .
```

### Run

```
./cpp-khun-poke
```

---

## ✅ Output Example

```
Player 1 expected value: -0.0567
Player 2 expected value: 0.0567

J rr  [0.82, 0.18]
Q rr  [0.03, 0.97]
K rr  [0.66, 0.34]
...
```

These match the known Nash equilibrium strategies for Kuhn Poker.

---

## ✅ Implementation Details

Main components:

| Component          | Description                                                  |
| ------------------ | ------------------------------------------------------------ |
| `cfr()`            | Recursive CFR function                                       |
| `InfoSet`          | Stores regret, reach probability, strategy, average strategy |
| `get_info_set()`   | Maps `(card, betting history)` to a unique InfoSet           |
| `next_strategy()`  | Applies regret-matching update after each CFR iteration      |
| `terminal_utils()` | Game payoff at terminal nodes                                |
| `chance_util()`    | Distributes chance over all card deals                       |


---

## ✅ References

* **H. W. Kuhn (1950):** Simplified poker game
* **Zinkevich et al. (2007):** Counterfactual Regret Minimization
* **Brown & Sandholm (2017):** CFR+ / Libratus
