# PASS Science Works — Unified Engineering Workbench

**R&D Internship 2026–2027 Engineering Assignment**  
**Author:** Gourav Sharma  
**Version:** v1.0.0-candidate (v0.1.0-prototype)  
**Target Platform:** Windows 11 x64  
**Main Repository:** [https://github.com/GouravSharma26/EngiWorkbench.git](https://github.com/GouravSharma26/EngiWorkbench.git)  
**Evidence Repository:** [https://github.com/GouravSharma26/DEMO22.git](https://github.com/GouravSharma26/DEMO22.git)  

---

## 1. Project Overview

The **Unified Engineering Workbench** is a high-performance, modular desktop engineering workstation designed for scientific analysis, geometry inspection, finite-element thermal simulation, multicore parallel benchmarking, embedded hardware telemetry monitoring, and transparent neural network backpropagation.

The application is built strictly following standard **C++17** for all core domain logic, numerical algorithms, file parsing, and multithreading, completely independent of the presentation framework. The graphical user interface is implemented using **Qt 6.8.2 Widgets** with custom high-performance 2D vector plots and an interactive 3D geometry viewport. The application ships as a fully self-contained Windows package launched via a native, double-clickable `Launch.exe` with zero third-party installer or admin requirements.

---

## 2. Key Capabilities Across All Five Views

### 1. Geometry View (Task B)
- **ASCII STL 3D Visualizer:** Interactive 3D renderer displaying triangular surface meshes with directional Lambertian shading, facet normal computation, and optional wireframe overlays.
- **Full Viewport Navigation:** Intuitive mouse controls:
  - Left Mouse Drag: Spherical camera orbit (azimuth and elevation).
  - Right / Middle Mouse Drag: Camera viewport panning.
  - Mouse Wheel: Continuous zoom in and out.
  - Reset View: Fit-to-view bounding-box centering.
- **Nodal Temperature Point Cloud:** Renders ANSYS-style 3D temperature coordinate fields with continuous smooth false-color colormap gradients (blue $\rightarrow$ cyan $\rightarrow$ green $\rightarrow$ yellow $\rightarrow$ red).
- **Physical Legend & Proximity Inspection:** Calibrated temperature legend bar displaying values in Celsius (°C). Clicking any point picks the closest node and presents its exact coordinates, ID, and temperature in an inspection table.
- **Provenance Transparency:** Bundled temperature data is prominently labeled **SYNTHETIC**; binary STLs are safely rejected with actionable diagnostics.

### 2. Thermal Analysis View (Task C)
- **1D Steady-State Heat Conduction Solver:** Solves the boundary value problem $-k A \frac{d^2T}{dx^2} = q A$ over a rod of length $L$, area $A$, conductivity $k$, volumetric generation $q$, and prescribed Dirichlet boundary temperatures $T(0) = T_0$ and $T(L) = T_L$.
- **Finite Element Assembly:** Assembles two-node linear element stiffness matrices $[K_e] = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}$ and consistent load vectors $\{f_e\} = \frac{q A h}{2} \begin{Bmatrix} 1 \\ 1 \end{Bmatrix}$.
- **Analytical Solution & Midpoint Error Verification:** Compares FE solutions against the exact analytical distribution $T(x) = T_0 + (T_L - T_0)\frac{x}{L} + \frac{q}{2k}(x L - x^2)$. Computes maximum absolute errors at element midpoints ($e_{\max,\text{mid}} = \frac{q}{8k} h^2$).
- **Mesh Convergence Study:** Refines meshes systematically ($N = 10, 20, 40, 80$), verifies exact quadratic asymptotic convergence ($O(h^2)$ with convergence ratio strictly equal to $4.0000$), and exports verified nodal, midpoint, and convergence CSV datasets.
- **Input Clamping:** UI controls automatically clamp physical parameters ($L \ge 0.01\text{ m}, A \ge 10^{-7}\text{ m}^2, k \ge 0.1\text{ W/(m}\cdot\text{K)}$) and provide actionable status messaging.

### 3. Performance View (Task D)
- **Reproducible Multicore Benchmark:** Benchmarks batches of independent thermal FEA cases generated deterministically using a fixed-seed Mersenne Twister RNG (`std::mt19937`).
- **Standard Library Worker Pool:** Powered by a bounded C++17 `ThreadPool` using condition-variable work queues and atomic dynamic scheduling.
- **Rigorous 5-Repetition Protocol:** Executes 1 unmeasured warm-up cycle followed by 5 timed repetitions per worker count ($p \in \{1, 2, 3, 4\}$). Computes median wall-clock time, speedup factor $S_p = T_1 / T_p$, and parallel efficiency $E_p = S_p / p$.
- **Numerical Output Agreement:** Verifies that serial and parallel execution order yields deterministic results and identical temperatures within $\max |T_{\text{serial}} - T_{\text{parallel}}| \le 10^{-8\ \circ}\text{C}$.
- **Responsive Execution & Safe Cancellation:** Workloads run on background threads; atomic cancellation halts pending tasks immediately without publishing partial or corrupted datasets.

### 4. Hardware View (Task E)
- **Embedded System Mocks:** Interfaces with three simulated physical adapters:
  1. *ESP32-DevKitC V4 Microcontroller* (Silicon Labs CP2102 USB-UART, 3.3V logic, internal temperature sensor).
  2. *Digilent Arty A7-35T FPGA* (FTDI FT2232HQ USB-JTAG/UART, Xilinx Artix-7, onboard auxiliary rail sensing).
  3. *Digilent JTAG-HS2 Debug Adapter* (High-speed JTAG programming and debug bridge; explicitly declared as an interface adapter, not a compute device).
- **Status & Electrical Badges:** Prominent `SIMULATED` badge on all device cards declaring that physical hardware is mocked. Displays logic standards (LVCMOS 3.3V), supply rails, and supported protocols (UART, SPI, I2C, JTAG).
- **Live Telemetry & Fault Recovery:** Real-time 1 Hz telemetry polling (die temperature, core voltage, packet throughput). Demonstrates simulated communication timeout faults and clean, safe recovery via reconnection without application restart.

### 5. Backpropagation View (Task F)
- **Single-Sample Tanh Neural Regression:** Transparent implementation of a single-neuron regression model ($x = 1.0, y = 2.0, \eta = 0.1$):
  $$z = w x + b, \quad h = \tanh(z), \quad \hat{y} = v h + c, \quad J = \frac{1}{2}(\hat{y} - y)^2$$
- **Exact Handwritten Analytical Gradients:** Evaluates exact partial derivatives prior to parameter mutation:
  $$\frac{\partial J}{\partial w} = \delta v (1 - h^2) x, \quad \frac{\partial J}{\partial b} = \delta v (1 - h^2), \quad \frac{\partial J}{\partial v} = \delta h, \quad \frac{\partial J}{\partial c} = \delta$$
- **Central Finite-Difference Verification:** Verifies analytical derivatives against central differences ($\epsilon = 10^{-6}$), confirming all errors satisfy $\Delta \le 10^{-5}$ (measured error: $1.13 \times 10^{-10}$).
- **Monotonic 100-Step Training:** Executes 100 simultaneous parameter updates ($\theta \leftarrow \theta - \eta \nabla J$) demonstrating smooth, monotonic loss convergence to $J_{100} = 1.70 \times 10^{-20}$ and $\hat{y} = 2.000000$. Exports full step-by-step training curves.

---

## 3. Technology Stack & Environment

| Component | Specification / Version |
|---|---|
| **Programming Language** | C++17 (ISO/IEC 14882:2017) |
| **GUI Framework** | Qt 6.8.2 Widgets & OpenGLWidgets (The Qt Company, LGPLv3) |
| **Compiler** | MSVC 19.44.35228.0 (Microsoft Visual Studio 2022 Community v17.14.38) |
| **Build System** | CMake 3.20+ with native `CMakePresets.json` |
| **Host OS** | Windows 11 Home / Pro x64 (Version 10.0.26100+) |
| **Windows SDK** | Windows 11 SDK 10.0.26100.0 / 10.0.22621.0 |
| **Launcher** | Native Win32 C++ application (`Launch.exe`, 37 KB, 0 Qt dependencies) |

---

## 4. Repository Architecture

The codebase strictly enforces clean architectural separation between domain logic, presentation, tests, evidence, and release distributions:

```text
EngiWorkbench/
├── .vscode/                                  # Editor workspace & task settings
├── build/                                    # Local CMake build tree (Git ignored)
├── data/                                     # Runtime test assets & example models
│   └── examples/                             # Sample ASCII STL & synthetic temperature CSV
├── dist/                                     # Distribution & packaging outputs (Git ignored)
│   ├── CandidateID_Workbench/                # Standalone zero-install application directory
│   ├── archives/                             # CandidateID_Workbench.zip & standalone archives
│   └── review_artifacts/                     # Preserved review artifacts
├── docs/                                     # System documentation, specifications & evidence
│   ├── evidence/                             # Curated acceptance evidence
│   │   ├── assignment/                       # Handwritten.pdf (2-page derivation assignment)
│   │   ├── csv/                              # 9 Verified numerical result CSVs
│   │   ├── screenshots/                      # 24 Full-resolution acceptance screenshots
│   │   ├── video/                            # Demonstration video & DEMO_VIDEO_INFORMATION.md
│   │   ├── phase3/                           # Phase 3 integration screenshots & checksum
│   │   ├── phase4/                           # Phase 4 release build & deployment screenshots
│   │   └── EVIDENCE_LOG.md                   # 20-Scenario manual acceptance log
│   ├── FINAL_REPORT.md                       # Comprehensive engineering report
│   ├── ARCHITECTURE.md                       # Subsystem decoupling & architecture specification
│   ├── REQUIREMENTS_TRACEABILITY.md          # 100% Traceability matrix across tasks A–F
│   └── WORKLOG.md                            # Complete chronological engineering worklog
├── packaging/                                # Native Win32 launcher resources, manifest & icon
├── results/                                  # Verified numerical benchmarks & execution logs
├── scripts/                                  # Build, packaging, and validation automation scripts
├── src/                                      # C++17 Application Source Code
│   ├── app/                                  # Qt 6 presentation layer & custom vector widgets
│   ├── core/                                 # Pure C++ computational domain (0 Qt dependencies)
│   ├── experiments/                          # Headless numerical benchmark & validation runner
│   └── launcher/                             # Win32 double-clickable wrapper (Launch.exe)
├── tests/                                    # Automated CTest test suites & QtTest GUI harness
│
├── .gitattributes                            # LF/CRLF text handling policies
├── .gitignore                                # Comprehensive ignore rules (builds, runtimes, *.mp4)
├── AGENTS.md                                 # Non-negotiable engineering constraints & instructions
├── CMakeLists.txt                            # Root CMake build configuration
├── CMakePresets.json                         # MSVC x64 Release & Debug presets
├── COMMIT_HISTORY.txt                        # Git commit trajectory & history log
├── EngiWorkbench.bundle                      # Offline repository bundle
├── MANIFEST.txt                              # Full candidate package manifest
├── OWNERSHIP.md                              # Code ownership & candidate originality declaration
├── PACK_CONTENTS.md                          # Package layout and structure index
├── plan.md                                   # Project implementation roadmap
├── README.md                                 # Architecture overview, user manual & build guide
└── THIRD_PARTY_NOTICES.md                    # Open-source licensing disclosures (Qt LGPLv3, MSVC)
```

---

## 5. Acceptance Evidence Suite

All acceptance evidence is permanently archived and cross-referenced in [`docs/evidence/EVIDENCE_LOG.md`](docs/evidence/EVIDENCE_LOG.md):

| Evidence Asset | Authoritative File Path | Properties / Verification | Status |
| :--- | :--- | :--- | :--- |
| **Demonstration Video** | [`docs/evidence/video/Screen Recording 2026-11-02 072049.mp4`](docs/evidence/video/) | **Duration:** `00:09:05` (9 min, 5 s)<br>**Format:** 1080p Full HD (1920×1080 @ 30 fps), H.264/AAC MP4<br>**Size:** 572,798,653 bytes<br>**SHA-256:** `FADE6064EA356941BE44F7CABEB540E2D0805DC19667C3E1EB6F0A8D30575A9C`<br>**Documentation:** [`docs/evidence/video/DEMO_VIDEO_INFORMATION.md`](docs/evidence/video/DEMO_VIDEO_INFORMATION.md) | **100% PLAYABLE** |
| **Handwritten Assignment** | [`docs/evidence/assignment/Handwritten.pdf`](docs/evidence/assignment/Handwritten.pdf) | **Pages:** 2 full handwritten pages<br>**Content:** Backprop chain-rule derivation and 1D FEA weak-form stiffness equation assembly<br>**Size:** 2,016,025 bytes<br>**SHA-256:** `81CA78F971ACDAF0571127AC85F24B47B8C52AE0B68AA52DF3D9D6423E8C194A` | **100% LEGIBLE** |
| **Acceptance Screenshots** | [`docs/evidence/screenshots/`](docs/evidence/screenshots/) | 24 full-resolution PNG screenshots covering all 5 workbench views, edge cases, input validation, and modal dialogs | **VERIFIED COMPLETE** |
| **Exported Numerical CSVs**| [`docs/evidence/csv/`](docs/evidence/csv/) | 9 verified CSV datasets generated directly from native C++ engines and exported via desktop UI | **VERIFIED VALID** |
| **Acceptance Evidence Log**| [`docs/evidence/EVIDENCE_LOG.md`](docs/evidence/EVIDENCE_LOG.md) | Comprehensive 20-scenario manual test matrix (`MAN-GEO-01` through `MAN-BP-04`) with inputs, expected vs observed, and pass/fail | **100% PASS** |

---

## 6. Standalone Execution (One-Click Offline Launch)

The standalone application is deployed in `dist/CandidateID_Workbench/` and archived in `dist/archives/CandidateID_Workbench.zip`.

### To Run:
1. Extract `dist/archives/CandidateID_Workbench.zip` to any local folder (paths with spaces supported, e.g., `C:\Engineering Workbench`).
2. Double-click `Launch.exe`.
3. The application opens immediately. **No administrator rights, terminal commands, compiler installations, or internet connections are required.**

---

## 7. Building and Running from Source

Prerequisites: Visual Studio 2022 (MSVC x64 C++ workload and Windows 11 SDK) and Qt 6.8.2 (`C:\Qt\6.8.2\msvc2022_64`).

### Release Configuration
```powershell
# 1. Configure
cmake --preset windows-msvc-x64-release

# 2. Build in parallel
cmake --build --preset windows-msvc-x64-release --parallel

# 3. Run all unit tests
ctest --preset windows-msvc-x64-release --output-on-failure
```

### Debug Configuration
```powershell
cmake --preset windows-msvc-x64-debug
cmake --build --preset windows-msvc-x64-debug --parallel
ctest --preset windows-msvc-x64-debug --output-on-failure
```

### Packaging & Verification Scripts
To reproduce the complete standalone distribution and verify archive integrity:
```powershell
# Build standalone runtime in dist/CandidateID_Workbench/
powershell -ExecutionPolicy Bypass -File scripts/package_release.ps1

# Package all-inclusive ZIP in dist/archives/CandidateID_Workbench.zip
powershell -ExecutionPolicy Bypass -File scripts/create_submission.ps1

# Perform entry-by-entry CRC32 integrity check
powershell -ExecutionPolicy Bypass -File scripts/verify_zip.ps1 -ZipPath dist/archives/CandidateID_Workbench.zip
```

---

## 8. Verified Automated Test Results (100% Pass)

Both Release and Debug configurations have been executed and verified via CTest:

```text
Test project C:/Gourav Sharma/work/EngiWorkbench/build/windows-msvc-x64-release
    Start 1: workbench_core_tests
1/8 Test #1: workbench_core_tests .............................   Passed    0.02 sec
    Start 2: workbench_thermal_tests
2/8 Test #2: workbench_thermal_tests ..........................   Passed    0.02 sec
    Start 3: workbench_parser_tests
3/8 Test #3: workbench_parser_tests ...........................   Passed    0.02 sec
    Start 4: workbench_performance_tests
4/8 Test #4: workbench_performance_tests ......................   Passed    0.28 sec
    Start 5: workbench_embedded_tests
5/8 Test #5: workbench_embedded_tests .........................   Passed    0.28 sec
    Start 6: workbench_backprop_tests
6/8 Test #6: workbench_backprop_tests .........................   Passed    0.02 sec
    Start 7: workbench_experiments_failure_exit_code_test
7/8 Test #7: workbench_experiments_failure_exit_code_test .....   Passed    0.41 sec
    Start 8: workbench_gui_tests
8/8 Test #8: workbench_gui_tests ..............................   Passed    1.81 sec

100% tests passed, 0 tests failed out of 8
Total Test time (real) =   2.86 sec
```

### Verification & Test Structure Summary:
- **CTest Release Suites:** 8/8 suites passed (100% pass rate) in 2.86 s of real execution time.
- **Phase 3 Integration Test Matrix:** 41 distinct end-to-end integration and verification rows documented in `docs/PHASE_3_TEST_MATRIX.csv`.
- **Standalone GUI Test Harness (`workbench_gui_tests`):** 9 actual GUI scenario test methods (`testMainWindowCreationAndTabs`, `testGeometryViewStlAndCsv`, `testGeometryViewInvalidFileHandling`, `testThermalViewSolveAndConvergence`, `testThermalViewParameterValidation`, `testPerformanceViewExecutionAndCancellation`, `testHardwareViewSimulatedDevices`, `testBackpropViewCalculationsAndTraining`, `testCrossViewSwitchingStability`) plus test setup (`initTestCase`) and cleanup (`cleanupTestCase`), yielding 11 passed QtTest entries (1810 ms).
- **Thermal Convergence Ratio:** $4.0000$ across $N=10, 20, 40, 80$ (exact theoretical $O(h^2)$).
- **Thermal Nodal Error:** $\le 10^{-9\ \circ}\text{C}$ (nodal superconvergence at mesh vertices).
- **Multicore Agreement:** $\max |T_{\text{serial}} - T_{\text{parallel}}| = 0.00 \times 10^{-00}\ ^\circ\text{C} \le 10^{-8\ \circ}\text{C}$ across 1,000 cases.
- **Multicore Benchmark Summary (`results/benchmark_1000cases_summary.csv`):**
  - $p=1$: Median $2.8618\text{ ms}$, Speedup $1.0000$, Efficiency $100.00\%$
  - $p=2$: Median $2.2033\text{ ms}$, Speedup $1.2989$, Efficiency $64.94\%$
  - $p=3$: Median $1.7766\text{ ms}$, Speedup $1.6108$, Efficiency $53.69\%$
  - $p=4$: Median $1.7974\text{ ms}$, Speedup $1.5922$, Efficiency $39.80\%$
- **Backprop Gradient Check:** Maximum absolute error $1.13 \times 10^{-10} \le 10^{-5}$ at $\epsilon = 10^{-6}$.
- **Backprop Training:** Monotonic loss descent from $J_0 = 1.182542$ to $J_{100} = 1.70 \times 10^{-20}$.

---

## 9. Supported File Formats & Data Conventions

1. **ASCII STL Geometry (`.stl`):** Standard ASCII format starting with `solid <name>` and containing `facet normal` followed by `outer loop` with 3 vertices. Binary STLs are detected by null-byte byte header scans and rejected safely.
2. **ANSYS-Style Nodal Temperature CSV (`.csv`):** Formatted with headers `node_id,x_m,y_m,z_m,temperature_C`. Supports flexible column ordering, whitespace tolerance, and actionable validation of duplicate IDs, non-finite values, and malformed records.
3. **CSV Result Exports:** All tables across Thermal, Performance, and Backprop views can be exported to user-selected writable files with standard CSV formatting.

---

## 10. Known Limitations & Mandatory Disclaimers

- **ASCII STL Only:** Binary STL files, native solid geometry formats (STEP/IGES), and proprietary CAD databases are deliberately out of scope and rejected with actionable messages.
- **1D Rod Thermal Model:** The FEA solver implements 1D longitudinal conduction with internal generation. It is not a 3D volumetric thermal solver.
- **Simulated Hardware Mocks:** Physical microcontrollers and FPGA boards are not detected or programmed; all device cards are simulated software mocks designed for telemetry visualization and fault-recovery demonstration. JTAG-HS2 is an interface adapter, not a compute board.
- **Synthetic Temperature Point Cloud:** The bundled point cloud dataset (`synthetic_temperature_nodes.csv`) is a synthetic test fixture generated for UI visualization. It is not an actual ANSYS simulation export.

---

## 11. Licence and Third-Party Compliance

- **Project Code:** Copyright (c) 2026 Gourav Sharma. Developed for PASS Science Works.
- **Qt 6 Framework:** Licensed under the GNU Lesser General Public License (LGPL) version 3. Shipped dynamically to permit library replacement.
- **Microsoft Visual C++ Runtime:** Distributed in accordance with the Microsoft Visual Studio 2022 Community REDIST terms.
- For complete licensing details, see [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and [`OWNERSHIP.md`](OWNERSHIP.md).
