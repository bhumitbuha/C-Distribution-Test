# Distribution Tester

A C++17 desktop application that generates random samples from statistical distributions and reports descriptive statistics, with a comparison between sequential and OpenMP-parallel generation.

Built as a final project for a C++ course, then cleaned up to demonstrate templates, polymorphism, parallel computing, and GUI programming with SFML and TGUI.

## Features

- **Two distributions**: continuous uniform and normal (Gaussian)
- **Statistics**: min, max, median, range, and a 20-bucket histogram
- **Parallel sample generation** with OpenMP, each thread using its own seeded `std::mt19937`
- **Side-by-side timing** so you can see when parallelism actually wins
- **TGUI/SFML GUI** for input and result display

## Architecture

```
TestDistribution<T>      // abstract base — owns samples + statistics
   |
   |-- UniformTest<T>    // fills samples from std::uniform_real_distribution
   |-- NormalTest<T>     // fills samples from std::normal_distribution
```

Subclasses are responsible only for sampling; all statistical computations live on the base class to avoid duplication. Both subclasses share the same parallel/sequential code path, gated on `_OPENMP`.

## Build

The build uses CMake and expects SFML 3, TGUI 1, and (optionally) OpenMP to be available. The easiest way on Windows is via [vcpkg](https://vcpkg.io):

```bash
vcpkg install sfml tgui
```

Then configure and build:

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

The resulting executable is at `build/Release/DistributionTester.exe` (Windows) or `build/DistributionTester` (Linux/macOS).

## Usage

1. Launch the app.
2. Enter the number of samples (e.g. `1000000`).
3. Enter the two parameters:
   - For **Uniform**: min and max.
   - For **Normal**: mean and standard deviation.
4. Click *Run Uniform Test* or *Run Normal Test*. Output shows statistics and timing for both the sequential and parallel runs.

## Notes / known limitations

- Parallel speed-up depends on `N`. For small sample counts the OpenMP run can be slower than sequential because of thread-launch overhead.
- The parallel path uses one `std::mt19937` per thread, seeded from `std::random_device` via `std::seed_seq`. Results are not reproducible across runs by design.
- `std::normal_distribution` carries internal state (Box-Muller pair cache), so the parallel `NormalTest` keeps a distinct distribution object per thread.

## Tech

C++17 - CMake - SFML 3 - TGUI 1 - OpenMP - templates - inheritance
