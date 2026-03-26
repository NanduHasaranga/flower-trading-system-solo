# Flower Trading System

A high-performance matching engine for a flower trading exchange built in C++. This project implements a central limit order book (CLOB) that reads incoming orders, processes them using a standard Price-Time Priority matching algorithm, and outputs execution reports.

## Architecture

- **Core (`include/Core/`)**: Contains basic domain entities like `Order`, `ExecutionReport`, and common `Types`.
- **Engine (`include/Engine/`)**: The core matching logic. The `Exchange` manages multiple `OrderBook` instances (one for each instrument/flower).
- **IO (`include/IO/`)**: Handles parsing input from CSV files (`orders.csv`) and formatting output to CSV (`execution.csv`).
- **Utils (`include/Utils/`)**: Utilities such as `MemoryPool` to efficiently manage object allocations during high-frequency trading simulations.

## Prerequisites

- A modern C++ compiler supporting **C++20** (e.g., GCC, Clang).

## Building

To compile the project with optimizations enabled, run the following command from the root directory of the project:

```bash
g++ -O3 -std=c++20 src/Engine/*.cpp src/IO/*.cpp src/Utils/*.cpp src/main.cpp -I include/ -o exchange_app
```

## Running the Application

1. Format your input orders and place them in `data/orders.csv`.
2. Execute the compiled application:
   ```bash
   .\exchange_app.exe
   ```
   _(For Unix-based environments, use `./exchange_app`)_
3. The matching engine will process the orders and generate `data/execution.csv` containing the executed trades and statuses.

## Project Status

_Work in Progress:_ This project is currently under active development. Core components like the matching engine logic, memory pooling, and file I/O are in the process of being fully implemented.
