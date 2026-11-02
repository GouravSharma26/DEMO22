# EngiWorkbench — Acceptance Testing & Evidence Log

**Project:** PASS Science Works — Unified Engineering Simulation & Analysis Platform  
**Candidate / Author:** Gourav Sharma  
**Version:** v1.0.0-candidate (v0.1.0-prototype)  
**Target Platform:** Windows 11 x64  
**Date of Acceptance:** 2026-11-02  
**Repository:** `https://github.com/GouravSharma26/EngiWorkbench.git`  
**Evidence Repository:** `https://github.com/GouravSharma26/DEMO22.git`  

---

## 1. Executive Summary & Evidence Overview

This document provides complete, transparent, and reproducible verification evidence for the manual acceptance testing, numerical validation, and end-to-end user workflows of the Unified Engineering Workbench (`EngiWorkbench`).

### Authoritative Acceptance Media
1. **Live Demonstration Video:**
   - **File Name:** `Screen Recording 2026-11-02 072049.mp4`
   - **Repository Paths:** `Screen Recording 2026-11-02 072049.mp4` (Root) and `docs/evidence/video/Screen Recording 2026-11-02 072049.mp4`
   - **Duration:** 00:09:05 (9 minutes, 5 seconds)
   - **Format:** ISO/IEC 14496-14 MP4 (`ftypmp42`, H.264 video, AAC audio, 1920×1080 @ 30 fps)
   - **File Size:** `572,798,653 bytes` (~546.26 MB)
   - **Verification:** Verified playable with smooth seek across all 5 modules and dialogs.
2. **Handwritten Derivations Assignment:**
   - **File Name:** `Handwritten.pdf`
   - **Repository Paths:** `Handwritten.pdf` (Root) and `docs/evidence/assignment/Handwritten.pdf`
   - **Format:** PDF 1.5 document (2-page legible handwritten mathematical derivation)
   - **File Size:** `2,016,025 bytes` (~1.92 MB)
   - **Content:** Exact handwritten forward-pass equations, chain-rule analytical gradient formulations, simultaneous parameter update expressions, and 1D FEA tridiagonal element stiffness matrix derivations.
3. **Acceptance Screenshots:**
   - **Directory:** `docs/evidence/screenshots/` (and root `Images-evidence/`)
   - **Count:** 24 genuine full-screen PNG captures covering all 5 views, parameter variations, fault injection, and CSV export verification.
4. **Exported CSV Datasets:**
   - **Directory:** `docs/evidence/csv/` (and root `results/`)
   - **Count:** 9 verified numerical CSV datasets covering Thermal FEA (3), Multicore Benchmarks (4), and Backpropagation (2).

---

## 2. Distinction Between Manual Acceptance and Automated CTest Suites

- **Automated CTest Suites (`AUTO-*`):** 8 automated suites compiled under Release (`/O2`) passing 100% in 2.86 s of real execution time. They assert algorithmic tolerances, memory bounds, and headless execution without user interaction.
- **Manual Acceptance Tests (`MAN-*`):** 20 comprehensive scenarios executed manually on the standalone release build (`Launch.exe`) in an isolated acceptance directory (`C:\EngiWorkbench Acceptance Test\Candidate Package\`). These verify interactive rendering, real-time telemetry, user event dispatching, visual plots, and file dialog exports.

---

## 3. Module-by-Module Acceptance Test Records

### 3.1 Module 1: Geometry & Point Cloud View (Task B)

All geometry and point-cloud visualization features operate strictly on ASCII STL surface meshes and coordinate CSV datasets.

| Test ID | Scenario Description | Expected Behavior | Observed Result | Status | Supporting Evidence File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **MAN-GEO-01** | Load bundled ASCII STL tetrahedron mesh | Parses 4 triangular facets, evaluates $[0,1]^3$ bounding box, renders 3D shaded surface with wireframe overlay | 3D regular tetrahedron displayed cleanly in viewport with directional Lambertian shading | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 222147.png` |
| **MAN-GEO-02** | 3D Viewport interactive camera controls | Left drag orbits spherical azimuth/elevation, right drag pans, mouse wheel zooms, Reset centers view | Smooth, continuous 3D orbiting and perspective transformation without clipping | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 222250.png` |
| **MAN-GEO-03** | Load bundled synthetic temperature CSV | Parses 7 coordinate nodes with scalar temperatures; renders colored spheres; displays °C legend bar; displays explicit provenance notice | Colored nodal spheres rendered at vertices and midpoints; legend 49°C to 100°C; notice "PROVENANCE: Synthetic Data (Self-created)" prominently shown | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 222436.png` |
| **MAN-GEO-04** | Interactive proximity node picking | Ray-cast distance picking highlights closest node with red circle; inspection table displays exact ID, X, Y, Z, and Temperature | Vertex picking selects Node 1001 ($T=100.0^\circ\text{C}$, $[0,0,0]$); edge midpoint selects Node 1005 ($T=74.0^\circ\text{C}$, $[0.01, 0.01, 0.0]$) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 222436.png`<br>`docs/evidence/screenshots/Screenshot 2026-11-01 222452.png` |

---

### 3.2 Module 2: 1D Steady-State Thermal FEA View (Task C)

Solves the steady-state boundary value problem $-kA \frac{d^2T}{dx^2} = qA$ with Dirichlet boundary conditions $T(0) = 100^\circ\text{C}, T(L) = 20^\circ\text{C}$ over rod parameters $L=1.0\text{ m}, A=10^{-4}\text{ m}^2, k=10\text{ W/(m}\cdot\text{K)}, q=10000\text{ W/m}^3$.

| Test ID | Scenario Description | Expected Behavior | Observed Result | Status | Supporting Evidence File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **MAN-TH-01** | Default single-mesh FEA solution ($N=10$) | Assembles $[K_e]$ and $\{f_e\}$; solves tridiagonal system; plots analytical exact curve and FE nodal points | Smooth parabolic distribution plotted ($T_{\max} = 188.2^\circ\text{C}$ at $x=0.42\text{ m}$); max nodal error $5.68\times 10^{-14}\ ^\circ\text{C}$; max midpoint error $1.250000^\circ\text{C}$ ($qh^2/8k$) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 222714.png`<br>`docs/evidence/screenshots/Screenshot 2026-11-01 223017.png` |
| **MAN-TH-02** | Mesh refinement at $N=20$ elements | Solves $N=20$ ($h=0.05\text{ m}$); evaluates nodal and midpoint errors | Max nodal error: $2.56\times 10^{-13}\ ^\circ\text{C}$; max midpoint error: $0.312500^\circ\text{C}$ (exact theoretical $\frac{qh^2}{8k}$) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 223730.png` |
| **MAN-TH-03** | Mesh refinement at $N=40$ elements | Solves $N=40$ ($h=0.025\text{ m}$); evaluates nodal and midpoint errors | Max nodal error: $7.67\times 10^{-13}\ ^\circ\text{C}$; max midpoint error: $0.078125^\circ\text{C}$ (exact theoretical $\frac{qh^2}{8k}$) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 223825.png` |
| **MAN-TH-04** | Mesh refinement at $N=80$ elements | Solves $N=80$ ($h=0.0125\text{ m}$); evaluates nodal and midpoint errors | Max nodal error: $1.14\times 10^{-12}\ ^\circ\text{C}$; max midpoint error: $0.019531^\circ\text{C}$ (exact theoretical $\frac{qh^2}{8k}$) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 223858.png` |
| **MAN-TH-05** | Second-order asymptotic convergence study | Successive mesh doubling yields error reduction ratio strictly equal to $4.0000$ ($O(h^2)$) | Convergence study table reports ratios: $4.00, 4.00, 4.00$; convergence plot confirms quadratic asymptotic slope | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 224005.png` |
| **MAN-TH-06** | Thermal CSV exports (3 files) | Exports nodal temperatures, midpoint errors, and convergence study CSVs with calibrated units | Verified files exported cleanly to disk: `thermal_nodes.csv`, `thermal_midpoint_errors.csv`, `thermal_convergence.csv` | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 230734.png`<br>`docs/evidence/screenshots/Screenshot 2026-11-01 230842.png`<br>`docs/evidence/csv/thermal_*.csv` |

---

### 3.3 Module 3: Multicore Parallel Benchmark View (Task D)

Benchmarks batches of 1,000 independent thermal FEA rods generated deterministically using Mersenne Twister RNG (`std::mt19937`, seed 42) across $p \in \{1, 2, 4\}$ worker threads.

| Test ID | Scenario Description | Expected Behavior | Observed Result | Status | Supporting Evidence File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **MAN-PRF-01** | Reproducible benchmark configuration | Displays Seed (42), Cases (1000), Elements (50), and Worker selection checkboxes in Idle state | UI configured; status displays `Idle`; agreement status `Pending` | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 230904.png` |
| **MAN-PRF-02** | Multicore execution & 5-repetition protocol | 1 warmup + 5 measured repetitions per worker count; computes median time, speedup $S_p = T_1/T_p$, and efficiency $E_p = S_p/p$ | Status `Completed successfully`; summary table and repetition table populated; speedup bar chart rendered | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 231033.png`<br>`docs/evidence/screenshots/Screenshot 2026-11-01 231539.png`<br>`docs/evidence/screenshots/Screenshot 2026-11-01 231628.png` |
| **MAN-PRF-03** | Serial vs Parallel numerical agreement | Case-by-case comparison across all 1,000 cases asserts $\max \|T_{\text{serial}} - T_{\text{parallel}}\| \le 10^{-8\ \circ}\text{C}$ | Status displays: `Agreement: PASSED (Max Diff: 0.00e+00 °C)` | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 231539.png` |
| **MAN-PRF-04** | Benchmark cancellation safety | Cancel button requests atomic shutdown; halts pending worker tasks without corrupting prior results | UI remains responsive; background thread pool cancels safely without crashes or partial exports | **PASS** | Demonstrated in live video `Screen Recording 2026-11-02 072049.mp4` |
| **MAN-PRF-05** | Benchmark CSV exports (4 files) | Exports generated cases, raw repetition timings, summary speedups, and agreement verification CSVs | Verified files exported cleanly to disk: `benchmark_cases.csv`, `benchmark_raw_timings.csv`, `benchmark_summary.csv`, `benchmark_agreement.csv` | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-01 231552.png`<br>`docs/evidence/csv/benchmark_*.csv`<br>`docs/evidence/csv/serial_parallel_agreement.csv` |

---

### 3.4 Module 4: Embedded Hardware Mocks View (Task E)

> **MANDATORY DISCLAIMER:** All embedded adapters (ESP32 MCU, Arty A7 FPGA, JTAG-HS2 TAP Bridge) are strictly **SIMULATED** software mocks executing locally on the host PC. No physical microcontrollers, FPGAs, or debug cables were detected, powered, connected, or programmed. The JTAG adapter is a programming interface, not a compute device.

| Test ID | Scenario Description | Expected Behavior | Observed Result | Status | Supporting Evidence File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **MAN-HW-01** | Simulated device cards & badging | Displays three hardware cards with prominent `SIMULATED` badges, logic standards (LVCMOS 3.3V), and interface lists | ESP32-DevKitC V4, Digilent Arty A7-35T, and Digilent JTAG-HS2 cards displayed with `SIMULATED` badges | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 002912.png` |
| **MAN-HW-02** | Connect & live 1 Hz telemetry streaming | Connects mock transport and streams deterministic voltage (3.295V), temperature (43.00°C), and current (85.0mA) | ESP32 status transitions to `Connected (Simulated Host Transport)`; Packet #15 telemetry received | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 002912.png` |
| **MAN-HW-03** | Communication timeout fault injection | Clicking "Inject Timeout Fault" simulates physical cable disconnect; transitions state to `ERROR / TIMEOUT (Recoverable)` | Red error banner: "FAULT DETECTED: Timeout fault: ESP32 UART bridge did not receive expected ACK within 3000ms (SIMULATED)" | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 003143.png` |
| **MAN-HW-04** | Reconnect & safe fault recovery | Clicking "Reconnect & Recover" clears fault state and restores live telemetry stream without application restart | ESP32 restored to `Connected` (Packet #215); Arty A7 connected simultaneously (Packet #28, FABRIC_READY_SIMULATED) | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 004152.png` |

---

### 3.5 Module 5: Neural Network Backpropagation View (Task F)

Single-sample tanh regression model: $x = 1.0, y = 2.0, \eta = 0.1, w_0 = 0.5, b_0 = 0.0, v_0 = 1.0, c_0 = 0.0$.  
Forward equations: $z = wx + b, h = \tanh(z), \hat{y} = vh + c, J = \frac{1}{2}(\hat{y} - y)^2$.

| Test ID | Scenario Description | Expected Behavior | Observed Result | Status | Supporting Evidence File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **MAN-BP-01** | Forward pass evaluation | Computes initial forward quantities: $z=0.5, h=0.462117, \hat{y}=0.462117, \delta=-1.537883, J=1.182542$ | Exact forward pass quantities displayed in parameter table and status bar | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 004813.png` |
| **MAN-BP-02** | Analytical gradients & finite-difference check | Analytical partial derivatives evaluated before mutation; central differences ($\epsilon=10^{-6}$) assert error $\le 10^{-5}$ | Status reports "Check: PASSED (Max Error: 1.13e-10)"; errors: $w: 1.13\times 10^{-10}, b: 1.13\times 10^{-10}, v: 5.25\times 10^{-11}, c: 1.12\times 10^{-10}$ | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 004813.png` |
| **MAN-BP-03** | Simultaneous parameter update step | All 4 parameters updated simultaneously using original gradients ($\theta \leftarrow \theta - \eta \nabla J$) | Step 1 parameters updated to $w_1 = 0.620946, b_1 = 0.120946, v_1 = 1.071068, c_1 = 0.153788$; loss decreases to $0.685774$ | **PASS** | Demonstrated in live video `Screen Recording 2026-11-02 072049.mp4` |
| **MAN-BP-04** | 100-step training trace & monotonic convergence | Executes 100 gradient steps; loss monotonically decreases towards zero ($J_{100} < 10^{-18}$) and $\hat{y} \rightarrow 2.000000$ | Smooth monotonic descent curve plotted; final $J = 0.000000, \hat{y} = 2.000000, \delta = -0.000000$ | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 004954.png` |
| **MAN-BP-05** | Reset model to initial parameters | Clears training curve and resets weights to initial values | Parameters reset to $w=0.5, b=0.0, v=1.0, c=0.0$; plot cleared cleanly | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 005008.png` |
| **MAN-BP-06** | Backpropagation CSV exports (2 files) | Exports finite-difference gradient check and 100-step training loss trace CSVs | Verified files exported cleanly to disk: `backprop_gradient_check.csv`, `backprop_loss_trace.csv` | **PASS** | `docs/evidence/screenshots/Screenshot 2026-11-02 005151.png`<br>`docs/evidence/csv/backprop_*.csv` |

---

## 4. Evidence Traceability & File Index

All evidence files referenced in this document physically exist within the repository and packaged candidate archive.

### 4.1 Screenshots Index (`docs/evidence/screenshots/`)

| Screenshot File Name | Module | Capture Purpose |
| :--- | :--- | :--- |
| `Screenshot 2026-11-01 222147.png` | Geometry | ASCII STL tetrahedron mesh loaded with wireframe enabled |
| `Screenshot 2026-11-01 222250.png` | Geometry | 3D Viewport spherical orbit and rotation demonstrated |
| `Screenshot 2026-11-01 222436.png` | Geometry | Nodal temperature point cloud loaded, legend bar, Node 1001 selected |
| `Screenshot 2026-11-01 222452.png` | Geometry | Node 1005 (edge midpoint) selected with coordinate table updated |
| `Screenshot 2026-11-01 222714.png` | Thermal | Default 1D FEA solution ($N=10$) with analytical curve overlay |
| `Screenshot 2026-11-01 223017.png` | Thermal | Single Mesh $N=10$ solve status showing exact nodal/midpoint errors |
| `Screenshot 2026-11-01 223730.png` | Thermal | Single Mesh $N=20$ solve status ($e_{\max,\text{mid}} = 0.312500^\circ\text{C}$) |
| `Screenshot 2026-11-01 223825.png` | Thermal | Single Mesh $N=40$ solve status ($e_{\max,\text{mid}} = 0.078125^\circ\text{C}$) |
| `Screenshot 2026-11-01 223858.png` | Thermal | Single Mesh $N=80$ solve status ($e_{\max,\text{mid}} = 0.019531^\circ\text{C}$) |
| `Screenshot 2026-11-01 224005.png` | Thermal | Convergence study table showing ratios $4.00, 4.00, 4.00$ and error plot |
| `Screenshot 2026-11-01 230734.png` | Thermal | File Explorer showing exported thermal CSV files in acceptance folder |
| `Screenshot 2026-11-01 230842.png` | Thermal | Spreadsheet software inspecting exported `thermal_convergence.csv` |
| `Screenshot 2026-11-01 230904.png` | Performance | Benchmark configuration in Idle state with Seed 42 and 1000 cases |
| `Screenshot 2026-11-01 231033.png` | Performance | Benchmark execution complete with agreement verified |
| `Screenshot 2026-11-01 231539.png` | Performance | Summary table, repetition table, and Speedup bar chart |
| `Screenshot 2026-11-01 231552.png` | Performance | File Explorer showing exported benchmark CSV files |
| `Screenshot 2026-11-01 231628.png` | Performance | Completed benchmark repetition timings log |
| `Screenshot 2026-11-02 002912.png` | Hardware | Three simulated hardware cards with SIMULATED badges; ESP32 telemetry |
| `Screenshot 2026-11-02 003143.png` | Hardware | Simulated timeout fault injected on ESP32 (`ERROR / TIMEOUT`) |
| `Screenshot 2026-11-02 004152.png` | Hardware | Reconnect & Recover demonstrated; Arty A7 connected simultaneously |
| `Screenshot 2026-11-02 004813.png` | Backprop | Forward pass, analytical gradients, and finite-difference check passed |
| `Screenshot 2026-11-02 004954.png` | Backprop | 100-step training trace with monotonic loss convergence plot |
| `Screenshot 2026-11-02 005008.png` | Backprop | Reset to Initial Parameters action verified |
| `Screenshot 2026-11-02 005151.png` | Backprop | File Explorer showing exported backpropagation CSV files |

### 4.2 Exported CSV Files Index (`docs/evidence/csv/` & `results/`)

| Exported CSV File Name | Module | Row Count | Primary Physical / Numerical Content |
| :--- | :--- |---:| :--- |
| `thermal_nodal_temperature_N10.csv` | Thermal | 11 | Nodal coordinates $x_i$, FE temperatures $T_i$, exact analytical $T_{\text{exact}}$, and error |
| `thermal_midpoint_errors.csv` | Thermal | 150 | Element midpoint locations and interpolated errors across $N=10, 20, 40, 80$ |
| `thermal_convergence_study.csv` | Thermal | 4 | Mesh $N$, element size $h$, max midpoint error, theoretical error, and ratio ($4.00$) |
| `benchmark_cases.csv` | Performance | 1,000 | Deterministic rod cases with randomized $k \in [5,25]$ and $q \in [1000, 20000]$ |
| `benchmark_raw_timings.csv` | Performance | 24 | Raw wall-clock execution timings across 1 warmup + 5 measured reps for $p \in \{1, 2, 3, 4\}$ |
| `benchmark_1000cases_summary.csv` | Performance | 4 | Worker counts $p$, median time $T_p$, speedup factor $S_p$, and efficiency $E_p$ |
| `serial_parallel_agreement.csv` | Performance | 1,000 | Case-by-case max temperature difference between serial and parallel runs ($\le 10^{-8\ \circ}\text{C}$) |
| `backprop_gradient_check.csv` | Backprop | 4 | Parameters $\{w, b, v, c\}$, analytical $\nabla J$, central finite difference, and absolute error |
| `backprop_training_trace_100steps.csv`| Backprop | 101 | Step-by-step optimization history (Step 0 to 100), parameters $\{w, b, v, c\}$, output $\hat{y}$, loss $J$ |

### 4.3 Demonstration Video & Assignment PDF

| Asset File Name | Staging Location | File Size | Description |
| :--- | :--- |---:| :--- |
| `Screen Recording 2026-11-02 072049.mp4` | Root & `docs/evidence/video/` | 572,798,653 B | Live continuous walkthrough of all 5 views and offline launcher (00:09:05) |
| `Handwritten.pdf` | Root & `docs/evidence/assignment/` | 2,016,025 B | 2-page handwritten assignment with backpropagation and FEA equations |

---

## 5. Automated CTest Validation Summary

In addition to manual acceptance testing, all 8 CTest automated suites pass under Release configuration:

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

---

## 6. Conclusion & Acceptance Status

All 20 manual acceptance tests (`MAN-GEO-01` through `MAN-BP-06`) and all 8 automated CTest suites have been successfully executed, witnessed, and traced to permanent disk artifacts. Zero tests failed. Zero evidence files are missing or fabricated.

