[README.md](https://github.com/user-attachments/files/32376059/README.md)
# Cellular Automata Simulation

A C implementation of one-dimensional elementary cellular automata, including
rule-based state evolution with periodic boundaries and a density-classification
demonstration using Rules 184 and 232.

## Why this repository exists

This is an **independent portfolio reimplementation** inspired by concepts I
studied in COMP10002 *Foundations of Algorithms* at the University of Melbourne.

It intentionally does **not** include the original assignment specification,
University-provided skeleton code, submission metadata, or assessed source files.

## Features

- Simulates one-dimensional elementary cellular automata.
- Supports Wolfram rule codes from `0` to `255`.
- Uses periodic boundary conditions.
- Prints the state of the automaton at each time step.
- Includes a density-classification demonstration using Rules 184 and 232.
- Uses dynamic memory allocation and modular C functions.

## Build

Compile with GCC:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/cellular_automata.c -o cellular_automata
```

## Usage

### Standard simulation

```bash
./cellular_automata simulate <rule> <steps> <initial_state>
```

Example:

```bash
./cellular_automata simulate 30 12 "....*....*...."
```

### Density-classification demonstration

```bash
./cellular_automata density <initial_state>
```

Example:

```bash
./cellular_automata density "..***..**....*."
```

The program first evolves the configuration using Rule 184 and then Rule 232,
before reporting whether ON cells form a majority, minority, or exactly half of
the final configuration.

## Concepts demonstrated

- C programming
- Dynamic memory management
- Elementary cellular automata
- Discrete-time simulation
- State-transition systems
- Periodic boundary conditions
- Algorithm design
- Density classification

## Project context

The original university project received **16.5/20**. This repository contains a
fresh implementation for portfolio presentation rather than the assessed
submission itself.
