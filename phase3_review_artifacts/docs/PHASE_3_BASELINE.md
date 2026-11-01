# EngiWorkbench — Phase 3 Baseline Audit Record

**Date:** 2026-11-01  
**Project:** EngiWorkbench — Unified Engineering Simulation & Analysis Platform  
**Repository:** `https://github.com/GouravSharma26/EngiWorkbench.git`  
**Phase:** 3 — Full Application Integration & End-to-End Verification  
**Author / Pair Programmer:** Antigravity (Google DeepMind)

---

## 1. Baseline Git Repository State

| Property | Value |
|---|---|
| **Active Branch** | `main` |
| **Commit Hash** | `336a153` (`fix(experiments): verify DualLogger close status and report stream failures before process exit`) |
| **Working Tree Status** | Clean (0 uncommitted files, 0 untracked files) |
| **Upstream Remote** | `origin/main` (local ahead by 5 commits: Phase 2 verified experimental runner and review fixes) |
| **Phase 2 Evidence Archive** | `EngiWorkbench_Phase2_Review_Artifacts.zip` (SHA-256: `3D06D21D510E4B22A140C8D47A0AD88E38B8D5DE37E7FEAA5BADDA1693A32710`, 69,513 bytes) |

---

## 2. Application Architecture & Linkage Audit

### 2.1 Backend / Computational Core (`workbench_core`)
- **Language Standard:** C++17 (`/std:c++17` on MSVC).
- **Library Target:** `workbench_core` (Static Library).
- **Qt Dependency:** Strictly independent of Qt. No presentation logic or GUI headers permitted.
- **Components:**
  - `domain/`: Shared domain data structures (`ThermalProblem`, `ThermalSolution`, `MeshData`, `TemperatureField`, `BenchmarkConfig`).
  - `thermal/solver.hpp`: 1D steady-state finite element solver, analytical solution, superconvergence midpoint evaluator, and mesh convergence study engine.
  - `io/stl_parser.hpp`: ASCII STL geometry reader with strict vertex parsing, normal checks, line-by-line validation, and error reporting.
  - `io/csv_parser.hpp`: ANSYS-style CSV nodal temperature parser with coordinate validation, tokenization, and temperature field extraction.
  - `performance/thread_pool.hpp` & `benchmark.hpp`: CPU worker pool, thread synchronization, atomic cancellation, deterministic batch problem solver, and steady-clock timing suite.
  - `embedded/adapter.hpp`: Abstract `IDeviceAdapter` interface and mock implementations for ESP32 microcontroller, Arty A7 FPGA, and Digilent JTAG-HS2 debug interface.
  - `backprop/model.hpp`: One-sample tanh neural network with forward inference, analytical gradients, central finite-difference checking, simultaneous SGD parameter updates, and 100-step training loop.

### 2.2 Desktop Application Target (`workbench_app`)
- **Framework:** Qt 6.8.2 MSVC 2022 64-bit (`Qt6::Core`, `Qt6::Widgets`, `Qt6::OpenGLWidgets`).
- **Core Linkage:** Links directly to `workbench_core`. All numerical calculations, file parsing, and benchmarks are delegated to `workbench_core`.
- **Target Type:** Native Windows executable (`workbench_app.exe`).
- **Main Window:** `workbench::app::MainWindow` (hosts 5 tabbed views, dark engineering theme, menu navigation, status bar provenance notices).

### 2.3 Experimental Runner Target (`workbench_experiments`)
- **Framework:** Pure C++17 console executable, links directly to `workbench_core`.
- **Core Linkage:** Identical linkage to `workbench_core` as `workbench_app`. Both targets invoke the exact same compiled numerical algorithms.

### 2.4 Standalone Win32 Launcher (`Launch.exe`)
- **Technology:** Pure C++17 Win32 launcher, zero external runtime dependencies. Forwarding working directory and command-line arguments to `workbench_app.exe`.

---

## 3. Build Configurations

| Configuration | Compiler Flags | CMake Preset | Target Output |
|---|---|---|---|
| **Release** | `/O2 /Ob2 /DNDEBUG` | `windows-msvc-x64-release` | `build/windows-msvc-x64-release/src/app/Release/workbench_app.exe` |
| **Debug** | `/MDd /Zi /Ob0 /Od /RTC1` | `windows-msvc-x64-debug` | `build/windows-msvc-x64-debug/src/app/Debug/workbench_app.exe` |

---

## 4. Existing Test Coverage (7 CTest Suites)

1. `workbench_core_tests`: Version strings, domain data bounds, and basic data flow.
2. `workbench_thermal_tests`: Boundary condition assignment, nodal superconvergence, midpoint error ratio ($4.0000$), physical parameter validation.
3. `workbench_parser_tests`: ASCII STL parsing, temperature CSV parsing, malformed files, empty files, missing files.
4. `workbench_performance_tests`: Deterministic case generation, serial/parallel numerical agreement ($\le 10^{-8\ \circ}\text{C}$), worker pool thread management.
5. `workbench_embedded_tests`: Discovery metadata, 3.3V reference checks, simulated telemetry generation, timeout fault injection, reconnect/recovery.
6. `workbench_backprop_tests`: Forward pass calculation, analytical vs finite difference gradient comparison ($\le 10^{-5}$), simultaneous SGD update, monotonic loss reduction over 100 steps.
7. `workbench_experiments_failure_exit_code_test`: Script-wrapped execution confirming non-zero exit code 1 on `--simulate-failure` with output isolated to build directory.

---

## 5. Existing Functionality in Each View

### 5.1 Geometry View (`GeometryView`)
- ASCII STL loading via file browser or bundled sample tetrahedron.
- Temperature CSV point cloud loading with synthetic provenance indicator.
- 3D custom viewport (`Viewport3D`) with interactive orbit (left-drag), pan (right/middle-drag), and zoom (wheel).
- Wireframe and solid rendering modes.
- Node picking with interactive highlight ring and data inspection table.
- Viewport auto-fit camera button.

### 5.2 Thermal Analysis View (`ThermalView`)
- Parameter inputs ($L, A, k, q, T_0, T_L, N$) with physical validation.
- Single mesh solve displaying nodal error and midpoint error.
- Automated convergence study ($N \in \{10, 20, 40, 80\}$).
- Dual plot widgets displaying temperature distribution (analytical vs FE) and convergence curve (computed vs theoretical $125/N^2$).
- CSV export for nodal temperatures, midpoint errors, and convergence table.

### 5.3 Performance View (`PerformanceView`)
- Workload configuration (RNG seed, case count, elements/case, worker checkboxes: 1, 2, 4).
- Background execution via `std::thread` preventing GUI thread freezes.
- Atomic cancellation with instant state restoration.
- Runtime measurements across 1 warm-up and 5 timed repetitions per worker count.
- Dynamic summary table (median, speedup, efficiency) and raw timings table.
- Serial vs parallel numerical agreement check ($\le 10^{-8\ \circ}\text{C}$).
- Speedup bar chart comparing measured speedup against ideal linear scaling.
- CSV export for cases, raw timings, summary, and agreement.

### 5.4 Hardware View (`HardwareView`)
- Three device cards: ESP32-DevKitC V4 (MCU), Digilent Arty A7-35T (FPGA), Digilent JTAG-HS2 (Debug Adapter).
- Prominent `SIMULATED` badge on every device card.
- Clear disclaimer noting JTAG is a debug interface, not a generic compute board.
- 1Hz live telemetry polling (temperature, voltage, current, packet counter).
- Timeout fault injection toggle with visual status indicator.
- Reconnect and recovery walkthrough.

### 5.5 Backpropagation View (`BackpropView`)
- Model description display (tanh activation, MSE loss, learning rate 0.1).
- Forward pass evaluation ($z, h, \hat{y}, \delta, J$).
- Analytical gradient calculation ($w, b, v, c$).
- Central finite-difference verification ($\epsilon = 10^{-6}$, tolerance $10^{-5}$).
- 1-step simultaneous SGD parameter update.
- 100-step training trace with live loss plot.
- Parameter table displaying values, analytical gradients, finite differences, and absolute errors.
- Reset button restoring initial parameters ($w=0.5, b=0.0, v=1.0, c=0.0$).
- CSV export for training trace and gradient check.

---

## 6. Known Limitations & Potential Defects to Investigate

1. **GUI Test Automation Gap:** Existing CTests cover core engines and console runner; there is currently no automated GUI test exercising `MainWindow` widget tree, tab switching, and action triggering via `QTest`.
2. **Relative File Paths in Bundled Data Buttons:** In `GeometryView`, "Load Bundled Sample Tetrahedron" uses relative path `"data/examples/sample_tetrahedron_ascii.stl"`. We must verify whether this works when the executable is launched from different working directories (e.g., from `build/` or standalone `dist/Workbench`).
3. **Screenshot / Evidence Capture:** Need to capture genuine evidence of each running view in `docs/evidence/phase3/`.
4. **CSV Export File Safety:** Need to confirm all GUI view CSV export routines handle file open failures and paths with spaces gracefully without crashing.

