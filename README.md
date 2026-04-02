# Flower Trading System

A high-performance matching engine for a flower trading exchange built in C++20. The project implements a central limit order book (CLOB) that reads incoming orders, matches them with price-time priority, and writes execution reports.

The repository supports two ways to run the application:

- CLI mode for headless execution without Qt.
- GUI mode for interactive use with a Qt Widgets front end.

Use a separate build directory for each mode. CMake stores the source directory in the build tree, so a folder configured from `traderApp/CMakeLists.txt` cannot be re-used for the root project without deleting its cache first.

## Architecture

- **Core (`include/Core/`)**: Domain entities such as `Order`, `ExecutionReport`, and `OrderReject`, plus validation and shared types.
- **Engine (`include/Engine/`)**: Matching logic. `Exchange` manages one `OrderBook` per instrument.
- **IO (`include/IO/`)**: CSV parsing and CSV output.
- **Utils (`include/Utils/`)**: Helpers such as `FixedString`, `TimeUtils`, and `OrderIdGenerator`.
- **GUI (`traderApp/`)**: Qt Widgets application for uploading an order file and viewing the generated report.

## Requirements

- **C++ compiler**: A compiler with C++20 support, such as GCC 11+, Clang 14+, or MSVC 19.30+.
- **CMake**: 3.20 or newer.
- **Qt for GUI only**: Qt 6.5 or newer with the Core and Widgets modules.
- **Matching toolchain**: Use the same compiler family for Qt and the build, for example MSVC with an MSVC Qt package or MinGW with a MinGW Qt package.
- The Qt prefix you pass to CMake must contain Qt6Config.cmake.

If you only want the CLI app, Qt is not required.

## Build and Run: CLI

The CLI target is built from the repository root and does not require Qt. The examples below use MinGW Makefiles on Windows.

```powershell
cmake -G "MinGW Makefiles" -S . -B build-cli -DBUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-cli
```

Run the executable from the build output directory:

```powershell
.\build-cli\exchange_app.exe
```

The CLI supports optional input and output paths:

```text
exchange_app [input_csv_path] [output_csv_path]
```

Defaults:

- Input: `data/orders.csv`
- Output: `data/execution.csv`

Examples:

```powershell
.\build-cli\exchange_app.exe
.\build-cli\exchange_app.exe data\example1.csv data\output.csv
```

## Build and Run: GUI

Enable the GUI build and point CMake to your Qt 6 installation. Replace the Qt path below with your local installation path. The Qt kit must match the compiler you use for the build.

If CMake cannot find Qt6, the path is wrong or Qt is not installed in that compiler kit.

```powershell
cmake -G "MinGW Makefiles" -S . -B build-gui -DBUILD_GUI=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.5.3\mingw_64"
cmake --build build-gui --target TraderApp
```

Run the GUI executable:

```powershell
.\build-gui\TraderApp.exe
```

On Windows, the build runs `windeployqt` after linking, so the Qt DLLs and required plugins are copied next to `TraderApp.exe` automatically.

GUI workflow:

- Upload an input CSV file.
- Process the orders.
- View the generated execution report.
- Download the report if needed.

The GUI generates `execution_report.csv` next to the selected input file.

## Scenario Tests

CSV-based scenario tests are available under `tests/cases/`.

```powershell
# Run tests only
./tests/run_csv_scenarios.ps1

# Build first, then run tests
./tests/run_csv_scenarios.ps1 -BuildBeforeRun
```

The script replaces `data/orders.csv` with each scenario input, runs the executable, and compares the generated output against the expected results.

## Performance Notes

- Release builds are optimized by default, with compiler-specific flags enabled in CMake (`-O3` for GCC/Clang and `/O2` for MSVC).
- The benchmark build path uses aggressive native optimization flags (`-O3`, `-march=native`, `-flto`, `-DNDEBUG`) for peak throughput testing.
- Timestamps use a thread-local per-millisecond cache (`utils::Timestamp`) to avoid repeated formatting work and heap allocations.
- Hot-path identifiers (`Order` and `ExecutionReport` IDs) use fixed-capacity stack storage via `FixedString<16>` instead of dynamic string allocations.
- CSV row parsing reuses a reserved line buffer and stores fields as `std::string_view`, minimizing per-row allocations and copies.
- Validation parses numeric fields with `std::from_chars` for fast, locale-independent conversion.
- Generated order IDs use an atomic counter with relaxed ordering and `std::to_chars` into a stack buffer.
- Matching emits execution events with in-place `emplace_back` construction and moves resting orders into the book to reduce copies.
- Book updates mutate top-level quantities in place and maintain price-level FIFO queues for efficient price-time matching.
- CSV output is batch-built in memory with pre-reserved capacity, per-line stack buffers, `std::memcpy`, `std::to_chars`, and a single bulk file write over a large stream buffer.
