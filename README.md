# Flower Trading System

A high-performance matching engine for a flower trading exchange built in C++20. This project implements a central limit order book (CLOB) that reads incoming orders, processes them using a standard Price-Time Priority matching algorithm, and outputs execution reports.

## Architecture

- **Core (`include/Core/`)**: Domain entities (`Order`, `ExecutionReport`, `OrderReject`), validation logic, and common types.
- **Engine (`include/Engine/`)**: The core matching logic. The `Exchange` manages multiple `OrderBook` instances (one per instrument).
- **IO (`include/IO/`)**: CSV parsing (`orders.csv`) and output formatting (`execution.csv`).
- **Utils (`include/Utils/`)**: Utilities including `FixedString` (stack-allocated strings), `Timestamp` (cached, zero-allocation timestamps), and `OrderIdGenerator`.

## Prerequisites

- A C++ compiler supporting **C++20** (e.g., GCC 11+, Clang 14+, MSVC 19.30+).
- **CMake 3.20+** (recommended) or manual compilation with `g++`.

## Building

### Using CMake (recommended)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Manual compilation

```bash
g++ -O3 -std=c++20 src/Core/*.cpp src/Engine/*.cpp src/IO/*.cpp src/Utils/*.cpp src/main.cpp -I include/ -o exchange_app
```

## Running the Application

The executable supports optional input and output paths:

```text
exchange_app [input_csv_path] [output_csv_path]
```

Defaults: input `data/orders.csv`, output `data/execution.csv`.

```bash
./exchange_app
./exchange_app data/example1.csv data/output.csv
```

## Running Scenario Tests

Eight CSV-based scenario tests are available under `tests/cases/`.

```powershell
# Run tests only
./tests/run_csv_scenarios.ps1

# Build first, then run tests
./tests/run_csv_scenarios.ps1 -BuildBeforeRun
```

The script replaces `data/orders.csv` with each scenario input, runs the executable, and compares the generated output against expected results.

## Performance Highlights

- **Zero-allocation timestamps** — cached per-millisecond via `thread_local`, no heap allocation.
- **Stack-allocated strings** — `ExecutionReport` uses `FixedString<16>` for IDs, eliminating per-report heap allocations.
- **Move semantics** — resting orders are moved into the order book, avoiding string copies.
- **Bulk I/O** — output is built in a pre-reserved buffer and written in a single `write()` syscall.
- **Fast formatting** — `std::to_chars` / `std::from_chars` for all numeric conversions (no locale overhead).
