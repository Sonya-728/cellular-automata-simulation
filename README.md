# Cellular Automata Simulation

A refactored C implementation of a one-dimensional elementary cellular automaton,
based on a project completed in **COMP10002 Foundations of Algorithms** at the
University of Melbourne.

The program simulates elementary cellular automata with periodic boundaries and
uses **Rules 184 and 232** to demonstrate the density-classification procedure.

## Portfolio context

The original assessed project received **16.5/20**. After receiving feedback, I
revisited the implementation and refactored it to improve data structures,
memory safety, and modularity while preserving the original algorithmic ideas.

This repository contains the refactored portfolio version rather than the
University-provided assignment skeleton or assessed submission.

## Key improvements after feedback

- Added a dedicated **tail pointer** to the linked list, reducing state insertion
  from O(n) traversal to **O(1)**.
- Linked the initial state directly into the same run history instead of keeping
  a duplicated, disconnected copy.
- Removed unnecessary copying when appending newly computed states by
  transferring ownership of each allocated state buffer into the linked list.
- Added complete cleanup of every allocated state, cell buffer, run structure,
  and automaton object.
- Ensured all allocated cell arrays include space for the null terminator.
- Removed use of uninitialised values and added validation for input and state
  access.
- Consolidated repeated linked-list traversal into a reusable `state_at`
  function.
- Separated simulation, history management, printing, counting, and density
  classification into clearer modules.
- Classified the original density from the final Rule-232 pattern rather than
  directly counting the original state.

## Concepts demonstrated

- C programming
- Dynamic memory management
- Linked data structures
- O(1) linked-list insertion
- Elementary cellular automata
- Discrete-time simulation
- State-transition systems
- Periodic boundary conditions
- Density classification
- Refactoring and memory-safety debugging

## Build

```bash
gcc -std=c11 -Wall -Wextra -Werror -pedantic src/cellular_automata.c -o cellular_automata
```

For an additional memory-safety check with GCC/Clang sanitizers:

```bash
gcc -std=c11 -Wall -Wextra -pedantic -fsanitize=address,undefined     -fno-omit-frame-pointer src/cellular_automata.c -o cellular_automata_asan
```

## Run

The program reads the automaton configuration from standard input.

```bash
./cellular_automata < examples/test0.txt
```

## Repository structure

```text
cellular-automata-simulation/
├── README.md
├── .gitignore
├── src/
│   └── cellular_automata.c
└── examples/
    ├── test0.txt
    └── sample_output.txt
```
