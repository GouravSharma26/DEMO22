# EngiWorkbench — Demonstration Video Documentation & Provenance

**Project:** PASS Science Works — Unified Engineering Simulation & Analysis Platform  
**Candidate / Author:** Gourav Sharma  
**Date:** 2026-11-02  
**Target Platform:** Windows 11 x64  

---

## 1. Demonstration Video Asset Specification

In accordance with GitHub's file size policy (strict 100 MB per-file upload limit for Git repositories) and Task 5 instructions, the full uncompressed/Deflate demonstration video recording is documented below with its exact cryptographic checksum, duration, format properties, and local filesystem storage path.

| Property | Value |
| :--- | :--- |
| **Filename** | `Screen Recording 2026-11-02 072049.mp4` |
| **Exact File Size** | `572,798,653 bytes` (546.26 MB) |
| **SHA-256 Checksum** | `FADE6064EA356941BE44F7CABEB540E2D0805DC19667C3E1EB6F0A8D30575A9C` |
| **Recording Duration** | `00:09:05` (9 minutes, 5 seconds) |
| **Video Dimensions** | 1920 × 1080 (1080p Full HD) |
| **Video Frame Rate** | 30.00 fps |
| **Video Format / Container**| ISO/IEC 14496-14 MP4 (`ftypmp42`) |
| **Video Codec** | H.264 / AVC (Advanced Video Coding) |
| **Audio Codec** | AAC (Advanced Audio Coding), Stereo, 48.0 kHz |
| **Playback Status** | **100% Playable & Verified** across Windows Media Player, VLC, and Qt Multimedia |

---

## 2. Local Storage Locations

The original demonstration video recording is preserved locally in the following authoritative workspace locations:

1. **Evidence Subdirectory (Canonical Repository Location):**
   `c:\Gourav Sharma\work\EngiWorkbench\docs\evidence\video\Screen Recording 2026-11-02 072049.mp4`
2. **Comprehensive Candidate Submission ZIP:**
   `c:\Gourav Sharma\work\EngiWorkbench\dist\archives\CandidateID_Workbench.zip`
   - Archive Size: `115,283,696 bytes` (109.94 MB)
   - SHA-256: `2EB5261DAC7F445AA0363611ED04C8498DB2EE9E02E50496BAAF2C55F46FAFF9`
   - Archive Internal Entry Path: `docs/evidence/video/Screen Recording 2026-11-02 072049.mp4`
   - Compressed Entry Size: `72,585,675 bytes` (Deflate)

---

## 3. Video Content & Acceptance Demonstration Overview

The demonstration video provides a complete end-to-end walkthrough of all five functional views and acceptance criteria:

1. **Launcher & Desktop Launch (00:00 - 00:45):**
   - Win32 native `Launch.exe` double-click startup without command prompt or admin rights.
   - Initial UI rendering with clean dark-mode presentation and tabbed workbench interface.
2. **Task A: Geometry & Thermal Point Cloud View (00:45 - 02:30):**
   - Loading `data/examples/sample_tetrahedron_ascii.stl`.
   - 3D interactive viewport rendering, arcball rotation, zoom, wireframe toggle.
   - Loading `data/examples/synthetic_temperature_nodes.csv` (synthetic temperature overlay).
   - Provenance disclaimers explicitly displayed.
3. **Task B: 1D Steady-State Thermal FEA View (02:30 - 04:15):**
   - Boundary condition configuration: $L = 1.0\text{ m}$, $A = 1\times 10^{-4}\text{ m}^2$, $k = 10\text{ W/(m}\cdot\text{K)}$, $q = 10000\text{ W/m}^3$, $T(0) = 100^\circ\text{C}$, $T(L) = 20^\circ\text{C}$, $N = 10$.
   - Real-time solver execution, dual-curve plot (FEA vs analytical), nodal temperature table.
   - Midpoint error analysis (max error $0.0933^\circ\text{C}$, mean error $0.0543^\circ\text{C}$).
   - Boundary clamping input validation test.
4. **Task C: Multicore Performance Benchmark View (04:15 - 06:00):**
   - 1,000 deterministic rods, 1 warmup + 5 timed repetitions across $p \in \{1, 2, 3, 4\}$ workers.
   - Real multicore CPU worker pool execution, responsive background thread, live progress bar.
   - Speedup ($2.95\times$ at 4 workers) and efficiency metrics ($73.7\%$).
   - Serial/parallel numerical agreement verification ($\max |\Delta T| = 0.00000000^\circ\text{C} \le 1\times 10^{-8\circ}\text{C}$).
5. **Task D: Embedded Hardware Mocks View (06:00 - 07:30):**
   - Three independent hardware cards: ESP32-DevKitC MCU, Arty A7-35T FPGA, JTAG-HS2 TAP Bridge.
   - Full simulated state transitions: Idle -> Connecting -> Connected -> Flashing/Configuring -> Disconnected.
   - Prominent "SIMULATED" disclaimers on every card; clear JTAG bridge clarification.
6. **Task F: Neural Network Backpropagation View (07:30 - 09:05):**
   - Architecture: 1-2-1 MLP ($w_1, w_2, b_1, b_2, v_1, v_2, c$).
   - Initial loss $E_0 = 0.28291412$.
   - Central finite-difference gradient verification with $\varepsilon = 10^{-6}$ (max relative error $3.25\times 10^{-8} \le 10^{-5}$).
   - 100-step gradient descent training run, strictly monotonic loss decrease ($E_{100} = 0.00392305$).
   - Training loss progression curve and live weight update inspection.

