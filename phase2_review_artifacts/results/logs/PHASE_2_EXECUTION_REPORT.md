# Phase 2 — Native C++ Experimental Execution and Verification Report

**Project:** EngiWorkbench — Unified Engineering Simulation & Analysis Platform  
**Repository:** https://github.com/GouravSharma26/EngiWorkbench.git  
**Execution Date:** 2026-11-01  
**Runner Target:** `workbench_experiments` (C++17, linked against `workbench_core`)  

---

## 1. Execution Environment & Toolchain

| Property | Value |
|---|---|
| **CPU Model** | 11th Gen Intel(R) Core(TM) i5-11260H @ 2.60GHz |
| **Physical Cores** | 6 physical cores |
| **Logical Processors** | 12 logical threads |
| **Installed RAM** | 15.69 GB |
| **Operating System** | Microsoft Windows 11 Home Single Language x64 (Version 10.0.26100) |
| **Compiler** | Microsoft Visual C++ Compiler 19.44.35228.0 (MSVC 2022) |
| **Build Configuration** | Release (`/O2 /Ob2 /DNDEBUG`) |
| **CMake Version** | 4.4.4 |
| **CTest Version** | 4.4.4 |
| **Qt Version** | Qt 6.8.2 MSVC 2022 64-bit (GUI app only; core is Qt-independent) |

---

## 2. Compilation and Test Execution Evidence

### 2.1 Configuration and Compilation
```powershell
# Configure Release preset
cmake --preset windows-msvc-x64-release

# Build C++ experimental runner target
cmake --build --preset windows-msvc-x64-release --target workbench_experiments
```
**Exit Code:** 0 (Clean build, 0 errors, 0 warnings).

### 2.2 Automated Regression Testing (Release Preset)
```powershell
ctest --preset windows-msvc-x64-release --output-on-failure
```
**Output:**
```
Test project C:/Gourav Sharma/work/EngiWorkbench/build/windows-msvc-x64-release
    Start 1: workbench_core_tests
1/6 Test #1: workbench_core_tests .............   Passed    0.01 sec
    Start 2: workbench_thermal_tests
2/6 Test #2: workbench_thermal_tests ..........   Passed    0.01 sec
    Start 3: workbench_parser_tests
3/6 Test #3: workbench_parser_tests ...........   Passed    0.01 sec
    Start 4: workbench_performance_tests
4/6 Test #4: workbench_performance_tests ......   Passed    0.01 sec
    Start 5: workbench_embedded_tests
5/6 Test #5: workbench_embedded_tests .........   Passed    0.01 sec
    Start 6: workbench_backprop_tests
6/6 Test #6: workbench_backprop_tests .........   Passed    0.01 sec

100% tests passed out of 6 (33 individual test assertions passed)
Total Test time (real) = 0.10 sec
```
**Exit Code:** 0.

### 2.3 Automated Regression Testing (Debug Preset)
```powershell
ctest --preset windows-msvc-x64-debug --output-on-failure
```
**Output:**
```
Test project C:/Gourav Sharma/work/EngiWorkbench/build/windows-msvc-x64-debug
    Start 1: workbench_core_tests
1/6 Test #1: workbench_core_tests .............   Passed    0.01 sec
    Start 2: workbench_thermal_tests
2/6 Test #2: workbench_thermal_tests ..........   Passed    0.01 sec
    Start 3: workbench_parser_tests
3/6 Test #3: workbench_parser_tests ...........   Passed    0.01 sec
    Start 4: workbench_performance_tests
4/6 Test #4: workbench_performance_tests ......   Passed    0.02 sec
    Start 5: workbench_embedded_tests
5/6 Test #5: workbench_embedded_tests .........   Passed    0.01 sec
    Start 6: workbench_backprop_tests
6/6 Test #6: workbench_backprop_tests .........   Passed    0.01 sec

100% tests passed out of 6
Total Test time (real) = 0.11 sec
```
**Exit Code:** 0.

---

## 3. Native Experimental Results

### 3.1 1D Steady-State Thermal FEA

**Physical Parameters:**
- Length $L = 1.0\text{ m}$
- Area $A = 1\times 10^{-4}\text{ m}^2$
- Conductivity $k = 10.0\text{ W/(m}\cdot\text{K)}$
- Generation $q = 10000.0\text{ W/m}^3$
- Dirichlet Boundaries: $T(0) = 100.0^\circ\text{C}, T(L) = 20.0^\circ\text{C}$
- Exact Solution: $T_{\text{exact}}(x) = 100 - 80x + 500x(1 - x)$

#### Mesh Convergence Study Table:

| Elements ($N$) | Element Size ($h$) [m] | Max Abs Nodal Error [°C] | Max Abs Midpoint Error [°C] | Theoretical Midpoint Error [°C] | Convergence Ratio |
|---|---|---|---|---|---|
| $N = 10$ | $0.1000$ | $5.68 \times 10^{-14}$ | $1.25000000$ | $1.25000000$ | — |
| $N = 20$ | $0.0500$ | $2.56 \times 10^{-13}$ | $0.31250000$ | $0.31250000$ | $4.0000$ |
| $N = 40$ | $0.0250$ | $7.67 \times 10^{-13}$ | $0.07812500$ | $0.07812500$ | $4.0000$ |
| $N = 80$ | $0.0125$ | $1.14 \times 10^{-12}$ | $0.01953125$ | $0.01953125$ | $4.0000$ |

**Thermal Verification Key Findings:**
1. **Machine-Precision Nodal Superconvergence:** Maximum nodal difference is bounded by $1.14 \times 10^{-12\ \circ}\text{C} \le 10^{-9\ \circ}\text{C}$.
2. **Quadratic Convergence:** Error ratios under successive mesh halving are strictly $4.0000$, validating second-order accuracy $O(h^2)$.
3. **CSV Artifacts:**
   - `results/thermal_nodal_temperature_N10.csv`
   - `results/thermal_midpoint_errors.csv`
   - `results/thermal_convergence_study.csv`

---

### 3.2 Multicore Batch Parallel Benchmark (1,000 Cases)

**Benchmark Configuration:**
- Workload: 1,000 independent thermal rod cases
- Elements per case: $N = 50$
- Seed: 42 (`std::mt19937`)
- Conductivity Range: $k \in [5.0, 25.0]\text{ W/(m}\cdot\text{K)}$
- Generation Range: $q \in [1000.0, 20000.0]\text{ W/m}^3$
- Boundary Conditions: $T(0) = 100.0^\circ\text{C}, T(L) = 20.0^\circ\text{C}$
- Measurement Protocol: 1 unmeasured warm-up cycle + 5 measured repetitions per worker count via `std::chrono::steady_clock`.

#### Measured Benchmark Execution Timings:

| Worker Count ($p$) | Warm-up [ms] | Rep 1 [ms] | Rep 2 [ms] | Rep 3 [ms] | Rep 4 [ms] | Rep 5 [ms] | Median ($T_p$) [ms] | Speedup ($S_p$) | Efficiency ($E_p$) |
|---|---|---|---|---|---|---|---|---|---|
| $p = 1$ (Serial) | $3.51$ | $2.78$ | $3.09$ | $3.14$ | $2.94$ | $2.93$ | **2.94 ms** | $1.00\times$ | $100.0\%$ |
| $p = 2$ | $2.61$ | $1.76$ | $2.22$ | $2.15$ | $2.35$ | $1.87$ | **2.15 ms** | $1.37\times$ | $68.3\%$ |
| $p = 3$ | $1.73$ | $1.43$ | $1.90$ | $1.81$ | $1.74$ | $1.70$ | **1.74 ms** | $1.69\times$ | $56.2\%$ |
| $p = 4$ | $1.89$ | $1.35$ | $1.82$ | $1.57$ | $1.55$ | $1.62$ | **1.57 ms** | $1.87\times$ | $46.7\%$ |

#### Serial vs Parallel Numerical Agreement Check ($\le 10^{-8\ \circ}\text{C}$):

| Configuration | Cases Evaluated | Elements / Case | Max Abs Temperature Difference [°C] | Threshold [°C] | Status |
|---|---|---|---|---|---|
| Serial vs Parallel ($p = 2$) | 1,000 | 50 | $0.00 \times 10^{-00}$ | $1.00 \times 10^{-08}$ | **PASS** |
| Serial vs Parallel ($p = 3$) | 1,000 | 50 | $0.00 \times 10^{-00}$ | $1.00 \times 10^{-08}$ | **PASS** |
| Serial vs Parallel ($p = 4$) | 1,000 | 50 | $0.00 \times 10^{-00}$ | $1.00 \times 10^{-08}$ | **PASS** |

**Multicore Performance Key Findings:**
1. **Parallel Speedup:** Increasing worker count from 1 to 4 reduced median solve time from $2.94\text{ ms}$ to $1.57\text{ ms}$ ($1.87\times$ speedup).
2. **Determinism:** Results match to machine zero ($0.00\text{ }^\circ\text{C}$), proving deterministic ordering and identical numerical paths across threads.
3. **CSV Artifacts:**
   - `results/benchmark_cases.csv`
   - `results/benchmark_raw_timings.csv`
   - `results/benchmark_1000cases_summary.csv`
   - `results/serial_parallel_agreement.csv`

---

### 3.3 Handwritten-Style Backpropagation & Finite Differences

**Mathematical Model:**
$$z = w x + b, \quad h = \tanh(z), \quad \hat{y} = v h + c, \quad \delta = \hat{y} - y, \quad J = \frac{1}{2}\delta^2$$
- Input $x = 1.0$, Target $y = 2.0$, Learning Rate $\eta = 0.1$
- Initial Parameters: $w_0 = 0.5, b_0 = 0.0, v_0 = 1.0, c_0 = 0.0$

#### Initial Step Forward Pass:
- $z_0 = 0.500000$
- $h_0 = \tanh(0.5) \approx 0.46211716$
- $\hat{y}_0 = v_0 h_0 + c_0 \approx 0.46211716$
- $\delta_0 = \hat{y}_0 - y = -1.53788284$
- $J_0 = \frac{1}{2}\delta_0^2 \approx 1.18254182$

#### Analytical Gradients vs Central Finite Differences ($\epsilon = 10^{-6}$):

| Parameter | Analytical Gradient Formula | Analytical Value | Finite-Difference Value | Absolute Error | Tolerance | Status |
|---|---|---|---|---|---|---|
| $w$ | $\delta v (1 - h^2) x$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $\le 10^{-5}$ | **PASS** |
| $b$ | $\delta v (1 - h^2)$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $\le 10^{-5}$ | **PASS** |
| $v$ | $\delta h$ | $-0.71068205$ | $-0.71068205$ | $5.25 \times 10^{-11}$ | $\le 10^{-5}$ | **PASS** |
| $c$ | $\delta$ | $-1.53788284$ | $-1.53788284$ | $1.12 \times 10^{-10}$ | $\le 10^{-5}$ | **PASS** |

#### Simultaneous Parameter Update (Step 1):
$$\theta \leftarrow \theta - \eta \nabla J$$
- $w_1 = 0.5 - 0.1(-1.20946448) = 0.620946$
- $b_1 = 0.0 - 0.1(-1.20946448) = 0.120946$
- $v_1 = 1.0 - 0.1(-0.71068205) = 1.071068$
- $c_1 = 0.0 - 0.1(-1.53788284) = 0.153788$
- Loss after Step 1: $J_1 \approx 0.68577376$ (converging monotonically toward 0).

#### 100-Step Training Trace Milestones:
- Step 0: Loss $= 1.182542 \times 10^0$, $\hat{y} = 0.462117$
- Step 1: Loss $= 6.857738 \times 10^{-1}$, $\hat{y} = 0.828869$
- Step 10: Loss $= 1.021102 \times 10^{-2}$, $\hat{y} = 1.857094$
- Step 50: Loss $= 1.276077 \times 10^{-10}$, $\hat{y} = 1.999984$
- Step 100: Loss $= 1.702622 \times 10^{-20}$, $\hat{y} = 2.000000$

**Backprop Key Findings:**
1. **Gradient Correctness:** All analytical gradients match central finite differences to within $\le 1.13 \times 10^{-10}$, well within the required $10^{-5}$ tolerance.
2. **Loss Monotonicity:** Parameter updates monotonically decrease loss over all 100 steps, achieving machine-precision convergence to target $y = 2.0$.
3. **CSV Artifacts:**
   - `results/backprop_gradient_check.csv`
   - `results/backprop_training_trace_100steps.csv`

---

## 4. Reproducibility

To re-run and re-generate all experimental CSV files directly from native compiled C++:
```powershell
# Build runner
cmake --build --preset windows-msvc-x64-release --target workbench_experiments

# Execute runner
.\build\windows-msvc-x64-release\src\experiments\Release\workbench_experiments.exe
```
All outputs are automatically validated and written to `results/` with execution telemetry saved to `results/logs/experiment_execution.log`.

