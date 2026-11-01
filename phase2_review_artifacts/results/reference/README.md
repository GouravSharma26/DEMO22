# Reference Analytical Calculations

This directory contains standalone analytical calculations and reference verification scripts used for independent cross-validation against the native C++ implementation in `workbench_core`.

## Files
- `reference_analytical_check.py`: Python analytical evaluation of 1D steady-state heat equation exact solution, central finite differences, and reference mathematical tanh derivations.

> **Notice:** The canonical experimental outputs are located in the parent directory (`results/`), generated exclusively by executing the native C++ compiled binary `workbench_experiments` linking `workbench_core`.

