# Fast Data Parser

![C++](https://img.shields.io/badge/C++-20-00599C?style=flat-square&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.15+-064F8C?style=flat-square&logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=flat-square&logo=windows)
![Threads](https://img.shields.io/badge/Multithreaded-8_cores-green?style=flat-square)
![Python](https://img.shields.io/badge/Python-3.x-blue?style=flat-square&logo=python)

A high-performance multithreaded data processing engine written in C++20, designed to handle huge amounts of data (millions of lines from transaction logs, telemetry, or server logs) in milliseconds

## Benchmarks

Dataset: 1,000,000+ financial transactions (~70 MB CSV file)

Language / Approach              | Time       | Speedup
---------------------------------|------------|----------
Python (standard csv module)     | ~15-40 s   |   1x
C++ (this project, 8 threads)    |  0.078 s   | ~100x

The C++ engine processes over 1 million rows in under 100 milliseconds

## Main Features

- **Memory Mapping (WinAPI):** The dataset is mapped directly into the process's virtual address space
. The OS manages all page caching, no data is copied into RAM

- **Zero-Copy Parsing:** Uses `std::string_view` as a lightweight window over already mapped memory 
. Zero heap allocations (`new`/`malloc`) during the entire parsing phase

- **Lock-Free Multithreading:** A custom Thread Pool splits the file into chunks aligned to newline boundaries 
. Each thread aggregates results into its own local hash map, no `std::mutex` in the hot path

- **Fast Number Conversion:** Uses C++17/20 `std::from_chars` for string to integer and string-to-double conversion, avoiding slow locale-aware functions like `std::stoi` or `std::stod`

## Technology Stack

* **Core language:** C++20 (`std::string_view`, `std::from_chars`, structured bindings)
* **Concurrency:** `std::thread`, `std::mutex`, `std::condition_variable` (custom Thread Pool)
* **File I/O:** WinAPI (`CreateFileMapping`, `MapViewOfFile`)
* **Data structures:** `std::unordered_map`, `std::array`, `std::string_view`
* **Build system:** CMake 3.15+
* **Data generation & baseline:** Python 3 (`csv`, `collections.defaultdict`)

---

## Quick Start

### Step 1. Clone the Repository
```bash
git clone https://github.com/Svatoi/Fast-Data-Parser.git
cd Fast-Data-Parser
```

### Step 2. Build the Project
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Step 3. Generate a Test Dataset
Use the provided Python script to generate a large CSV file for testing:
```bash
python scripts/generate_data.py 1000000 data.csv
```
This will create `data.csv` (~70 MB) with 1,000,000 rows of randomized financial transaction data

### Step 4. Run the Parser
```bash
.\build\Release\FastDataParser.exe data.csv
```

Expected output:
```
Processing 69 MB across 8 threads...
========================================
Done in: 0.0782149 seconds
Unique users with SUCCESS: 10000
========================================
Top 5 Users:
User 2142: 263090
User 7244: 262929
...
```

---

## Verify Correctness Against Python Baseline

To confirm the C++ output is correct, run the Python baseline on the same file and compare the Top 5 results, they must match exactly:
```bash
python scripts/process_data.py data.csv
```

---

## Project Architecture

```text
Fast-Data-Parser/
├── src/
│   ├── main.cpp                # Orchestrator: chunking, thread spawning, global merge
│   ├── MemoryMappedFile.h      # RAII wrapper for WinAPI memory-mapped file I/O
│   ├── ThreadPool.h            # Custom thread pool (mutex + condition_variable)
│   └── CsvParser.h             # Zero-copy CSV row parser (string_view + from_chars)
├── scripts/
│   ├── generate_data.py        # Generates large test CSV datasets
│   └── process_data.py         # Python baseline for benchmark comparison
├── CMakeLists.txt              # Build configuration (C++20, Release optimization flags)
└── PROJECT_DESCRIPTION.txt     # Project description
```
