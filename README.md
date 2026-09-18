# Cellular Automata Simulation

A refactored C implementation of a one-dimensional elementary cellular automaton,
based on a project completed in **COMP10002 Foundations of Algorithms** at the
University of Melbourne.

The program simulates elementary cellular automata with periodic boundaries and
uses **Rules 184 and 232** to demonstrate the density-classification procedure.

## Portfolio Context

The original assessed project received **16.5/20**. After receiving feedback, I
revisited the implementation and refactored it to improve data structures,
memory safety, efficiency, and modularity while preserving the original
algorithmic ideas.

This repository contains the refactored portfolio version rather than the
University-provided assignment skeleton or assessed submission.

## Features

- Simulates one-dimensional elementary cellular automata.
- Supports elementary cellular automaton rule codes from `0` to `255`.
- Uses periodic boundary conditions.
- Stores the evolution of the automaton as a linked sequence of states.
- Includes a density-classification demonstration using Rules 184 and 232.
- Uses dynamic memory allocation and modular C functions.

## Key Improvements After Feedback

- Added a dedicated **tail pointer** to the linked list, reducing state insertion
  from O(n) traversal to **O(1)**.
- Linked the initial state directly into the same run history instead of keeping
  a duplicated or disconnected copy.
- Removed unnecessary copying when appending newly computed states by
  transferring ownership of each allocated state buffer into the linked list.
- Added complete cleanup of allocated state nodes and cell buffers.
- Ensured all allocated cell arrays include space for the null terminator.
- Removed use of uninitialised values and added validation for state access.
- Consolidated repeated linked-list traversal into reusable helper functions.
- Separated simulation, history management, printing, counting, and density
  classification into clearer modules.
- Used the final Rule-232 pattern to infer the density classification rather than
  directly recounting the original configuration.

## Concepts Demonstrated

- C programming
- Dynamic memory management
- Linked data structures
- O(1) linked-list insertion
- Elementary cellular automata
- Discrete-time simulation
- State-transition systems
- Periodic boundary conditions
- Density classification
- Algorithm design
- Refactoring and memory-safety debugging

## Build

Compile with GCC:

```bash
gcc -std=c11 -Wall -Wextra -Werror -pedantic src/cellular_automata.c -o cellular_automata