# EngiWorkbench — Pack Contents & Submission Archive Index

**Project:** PASS Science Works — Unified Engineering Simulation & Analysis Platform  
**Candidate / Author:** Gourav Sharma  
**Version:** v1.0.0-candidate (v0.1.0-prototype)  
**Target Platform:** Windows 11 x64 (MSVC 2022, Qt 6.8.2)  
**Package:** `CandidateID_Workbench.zip` (All-Inclusive Unified Candidate Submission Package)  

---

## 1. Top-Level Package Architecture

The package extracts directly into the target directory with the standalone application at the root, preserving one-click offline execution alongside complete C++ source, tests, build configurations, and verified evidence.

```text
CandidateID_Workbench/
├── Launch.exe                                  # Native Win32 Launcher (Root Entry Point, 0 Qt dependencies)
├── workbench_app.exe                           # Main Engineering Workstation Application
├── workbench_experiments.exe                   # Native C++ Headless Verification Runner
├── workbench_gui_tests.exe                     # Automated QtTest GUI Integration Test Harness
├── CMakeLists.txt                              # Top-level CMake build configuration
├── CMakePresets.json                           # Native CMake presets (Release / Debug)
├── README.md                                   # User guide, build instructions, and architecture manual
├── MANIFEST.txt                                # Comprehensive package manifest & disclaimers
├── PACK_CONTENTS.md                            # Complete package contents directory index
├── OWNERSHIP.md                                # Intellectual property & candidate originality statement
├── THIRD_PARTY_NOTICES.md                      # Open-source licenses (Qt 6.8.2 LGPLv3, MSVC Redist)
├── COMMIT_HISTORY.txt                          # Git commit graph and revision log
├── EngiWorkbench.bundle                        # Offline Git repository bundle
├── Handwritten.pdf                             # Handwritten mathematical derivation assignment
├── Screen Recording 2026-11-02 072049.mp4      # Live demonstration video recording (00:09:05)
│
├── [Qt 6.8.2 Shared Dynamic Libraries]        # Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll, Qt6OpenGL.dll, etc.
├── [MSVC 2022 x64 Runtimes]                    # msvcp140.dll, vcruntime140.dll, vcruntime140_1.dll
├── [Graphics Subsystem Fallbacks]              # opengl32sw.dll, D3Dcompiler_47.dll
│
├── platforms/                                  # Mandatory Qt platform plugin (qwindows.dll)
├── styles/                                     # Qt widget desktop theming plugin
├── imageformats/                               # Qt image format plugins (qsvg.dll, qjpeg.dll, etc.)
├── generic/, iconengines/, networkinformation/, tls/ # Supporting Qt runtime plugins
│
├── src/                                        # COMPLETE C++17 SOURCE CODE
│   ├── core/                                   # Pure C++17 computational engine (Zero Qt headers)
│   ├── app/                                    # Qt 6 presentation layer & custom vector widgets
│   ├── experiments/                            # Headless numerical benchmark runner
│   └── launcher/                               # Native Win32 API process creation wrapper
│
├── tests/                                      # AUTOMATED CTEST TEST SUITES (8 suites, 100% pass)
│   ├── CMakeLists.txt
│   ├── test_main.cpp                           # Catch2 entry point
│   ├── test_parsers.cpp                        # ASCII STL and CSV parser unit tests
│   ├── test_thermal.cpp                        # 1D FEA solver, midpoint accuracy & convergence tests
│   ├── test_performance.cpp                    # ThreadPool, 1000-case RNG, serial/parallel agreement
│   ├── test_embedded.cpp                       # ESP32, Arty A7, JTAG mock adapters state machines
│   ├── test_backprop.cpp                       # Analytical gradients, finite differences, loss descent
│   ├── test_gui_integration.cpp                # 9 GUI scenario test methods + setup/cleanup (11 QtTest entries)
│   └── verify_exit_code.cmake                  # CTest failure-exit-code test script
│
├── results/                                    # NATIVE VERIFIED NUMERICAL CSV DATASETS & LOGS
│   ├── thermal_nodal_temperature_N10.csv       # FEA nodal temperatures vs exact analytical
│   ├── thermal_midpoint_errors.csv             # Element midpoint error distributions
│   ├── thermal_convergence_study.csv           # Grid refinement study N=10 to 80 (ratio 4.00)
│   ├── benchmark_cases.csv                     # 1,000 deterministic benchmark rod cases
│   ├── benchmark_raw_timings.csv               # 1 warmup + 5 measured reps across 1..4 workers
│   ├── benchmark_1000cases_summary.csv         # Medians, speedup, and efficiency metrics
│   ├── serial_parallel_agreement.csv           # Serial vs parallel max difference (<= 1e-8 °C)
│   ├── backprop_gradient_check.csv             # Analytical vs finite difference gradients
│   ├── backprop_training_trace_100steps.csv    # 100-step monotonic loss convergence trace
│   ├── reference/                              # Independent Python analytical reference model
│   └── logs/                                   # Headless execution logs and phase reports
│
├── data/                                       # EXAMPLE ASSETS
│   └── examples/
│       ├── sample_tetrahedron_ascii.stl        # Bundled 4-facet ASCII STL geometry
│       ├── synthetic_temperature_nodes.csv     # Bundled 7-node synthetic temperature cloud
│       ├── manifest.json                       # Data provenance and schema declarations
│       └── README.md
│
├── docs/                                       # COMPLETE DOCUMENTATION & VERIFICATION EVIDENCE
│   ├── FINAL_REPORT.md                         # Final comprehensive engineering report
│   ├── DEMO_SCRIPT.md                          # Live demonstration walkthrough script
│   ├── AI_USE_LOG.md                           # AI assistance disclosure and prompt audit
│   ├── ARCHITECTURE.md                         # Architecture and module decoupling specification
│   ├── MODEL_AND_TECH_DECISIONS.md             # Mathematical models and technical decisions
│   ├── REQUIREMENTS_TRACEABILITY.md            # Comprehensive requirement mapping matrix
│   ├── DEVELOPMENT_CHECKLIST.md                # Engineering completion checklist
│   ├── VALIDATION_PROTOCOL.md                  # Verification and testing protocol
│   ├── PACKAGING_AND_RELEASE.md                # Standalone packaging specification
│   ├── WORKLOG.md                              # Chronological development worklog
│   ├── PHASE_2_EXECUTION_REPORT.md             # Phase 2 computational core report
│   ├── PHASE_3_INTEGRATION_VERIFICATION.md     # Phase 3 application integration report
│   ├── PHASE_3_TEST_MATRIX.csv                 # Phase 3 41-row integration test matrix
│   ├── PHASE_4_RELEASE_VERIFICATION.md         # Phase 4 release build verification
│   ├── PHASE_4_PACKAGE_AUDIT.md                # Phase 4 package content audit
│   └── evidence/                               # FORMAL ACCEPTANCE EVIDENCE
│       ├── EVIDENCE_LOG.md                     # Comprehensive 20-scenario manual acceptance log
│       ├── screenshots/                        # 24 full-screen acceptance screenshots
│       ├── csv/                                # 9 exported numerical CSV datasets
│       ├── video/                              # Demonstration video (Screen Recording 2026-11-02 072049.mp4)
│       ├── assignment/                         # Handwritten assignment (Handwritten.pdf)
│       ├── phase3/                             # Phase 3 application screenshots
│       └── phase4/                             # Phase 4 release build and CTest screenshots
│
└── packaging/                                  # Native Win32 launcher resources, manifest & app icon
```

---

## 2. Evidence Verification Summary

| Evidence Type | Storage Location | Count / Size | Status |
| :--- | :--- |---:| :--- |
| **Demonstration Video** | `Screen Recording 2026-11-02 072049.mp4` & `docs/evidence/video/` | 572,798,653 B (00:09:05) | **VERIFIED PLAYABLE** |
| **Handwritten Assignment** | `Handwritten.pdf` & `docs/evidence/assignment/` | 2,016,025 B (2 pages) | **VERIFIED LEGIBLE** |
| **Acceptance Screenshots** | `docs/evidence/screenshots/` (and `Images-evidence/`) | 24 PNG files | **VERIFIED COMPLETE** |
| **Exported CSV Datasets** | `docs/evidence/csv/` (and `results/`) | 9 CSV files | **VERIFIED VALID** |
| **Acceptance Evidence Log**| `docs/evidence/EVIDENCE_LOG.md` | 1 document | **VERIFIED COMPLETE** |
| **Automated CTest Suites** | `tests/` (8 suites) | 8/8 passed (2.86 s) | **100% PASS** |
| **Standalone Launch** | `Launch.exe` | 37 KB (0 Qt dependencies) | **VERIFIED OFFLINE** |
