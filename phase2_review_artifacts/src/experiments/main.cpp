#include "workbench/thermal/solver.hpp"
#include "workbench/performance/benchmark.hpp"
#include "workbench/backprop/model.hpp"
#include "workbench/domain/thermal.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <numeric>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <sysinfoapi.h>
#endif

namespace fs = std::filesystem;

class DualLogger {
public:
    DualLogger(const fs::path& log_path) : log_path_(log_path) {
        log_file_.open(log_path, std::ios::out | std::ios::trunc);
    }

    ~DualLogger() {
        if (!is_closed_) {
            close();
        }
    }

    template <typename T>
    DualLogger& operator<<(const T& val) {
        std::cout << val;
        if (!is_closed_ && log_file_.is_open()) {
            log_file_ << val;
        }
        return *this;
    }

    DualLogger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(std::cout);
        if (!is_closed_ && log_file_.is_open()) {
            manip(log_file_);
        }
        return *this;
    }

    void flush() {
        std::cout.flush();
        if (!is_closed_ && log_file_.is_open()) {
            log_file_.flush();
        }
    }

    bool close() {
        if (!is_closed_) {
            is_closed_ = true;
            if (log_file_.is_open()) {
                log_file_.flush();
                bool good = log_file_.good();
                log_file_.close();
                close_successful_ = good && !log_file_.fail();
                return close_successful_;
            }
            close_successful_ = false;
            return close_successful_;
        }
        return close_successful_;
    }

    bool is_good() const {
        return !is_closed_ && log_file_.is_open() && log_file_.good();
    }

    const fs::path& path() const {
        return log_path_;
    }

private:
    fs::path log_path_;
    std::ofstream log_file_;
    bool is_closed_{false};
    bool close_successful_{false};
};

struct SystemInfo {
    std::string cpu_model{"Unknown"};
    unsigned int physical_cores{0};
    unsigned int logical_processors{0};
    double total_ram_gb{0.0};
    std::string os_version{"Windows"};
    std::string compiler_info{"MSVC"};
    std::string build_config{"Release"};
};

SystemInfo query_system_info() {
    SystemInfo info;
#ifdef _WIN32
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    info.logical_processors = sys_info.dwNumberOfProcessors;

    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(mem_status);
    if (GlobalMemoryStatusEx(&mem_status)) {
        info.total_ram_gb = static_cast<double>(mem_status.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
    }

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD buffer_size = sizeof(buffer);
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &buffer_size) == ERROR_SUCCESS) {
            info.cpu_model = std::string(buffer);
        }
        RegCloseKey(hKey);
    }

    DWORD length = 0;
    GetLogicalProcessorInformation(nullptr, &length);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &length)) {
            unsigned int physical = 0;
            for (const auto& item : buffer) {
                if (item.Relationship == RelationProcessorCore) {
                    ++physical;
                }
            }
            info.physical_cores = physical;
        }
    }
    if (info.physical_cores == 0) {
        info.physical_cores = info.logical_processors / 2;
    }

    info.os_version = "Microsoft Windows 11 x64";
#else
    info.logical_processors = std::thread::hardware_concurrency();
    info.physical_cores = info.logical_processors;
#endif

#if defined(_MSC_VER)
    std::ostringstream oss;
    oss << "MSVC " << _MSC_VER << " (" << _MSC_FULL_VER << ")";
    info.compiler_info = oss.str();
#elif defined(__clang__)
    info.compiler_info = "Clang " __clang_version__;
#elif defined(__GNUC__)
    info.compiler_info = "GCC " __VERSION__;
#endif

#if defined(NDEBUG)
    info.build_config = "Release (/O2)";
#else
    info.build_config = "Debug";
#endif

    return info;
}

fs::path find_results_dir(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--output-dir" && i + 1 < argc) {
            return fs::path(argv[i + 1]);
        }
    }

    fs::path cur = fs::current_path();
    for (int depth = 0; depth < 5; ++depth) {
        if (fs::exists(cur / "results") && fs::is_directory(cur / "results")) {
            return cur / "results";
        }
        if (fs::exists(cur / "CMakeLists.txt")) {
            return cur / "results";
        }
        if (cur.has_parent_path() && cur != cur.parent_path()) {
            cur = cur.parent_path();
        } else {
            break;
        }
    }
    return fs::current_path() / "results";
}

/// Safely writes content to a file, verifying open, stream state after flush, and successful close.
bool write_file_safely(const fs::path& file_path, const std::string& content, std::string& err_msg) {
    std::ofstream f(file_path, std::ios::out | std::ios::trunc);
    if (!f.is_open()) {
        err_msg = "Failed to open file for writing: " + file_path.string();
        return false;
    }
    f << content;
    f.flush();
    if (!f.good()) {
        err_msg = "Stream error occurred while writing to: " + file_path.string();
        f.close();
        return false;
    }
    f.close();
    if (f.fail()) {
        err_msg = "Stream error occurred while closing file: " + file_path.string();
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    bool simulate_failure = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--simulate-failure") {
            simulate_failure = true;
        }
    }

    bool all_validations_passed = true;
    std::vector<std::string> validation_failures;

    auto record_failure = [&](const std::string& reason) {
        all_validations_passed = false;
        validation_failures.push_back(reason);
    };

    try {
        fs::path results_dir = find_results_dir(argc, argv);
        fs::path logs_dir = results_dir / "logs";
        fs::path ref_dir = results_dir / "reference";

        fs::create_directories(results_dir);
        fs::create_directories(logs_dir);
        fs::create_directories(ref_dir);

        DualLogger logger(logs_dir / "experiment_execution.log");
        if (!logger.is_good()) {
            record_failure("Failed to initialize experiment execution log file at: " + (logs_dir / "experiment_execution.log").string());
        }

        logger << "===============================================================================\n";
        logger << "  EngiWorkbench - Native C++ Experimental Execution and Verification Runner\n";
        logger << "===============================================================================\n\n";

        if (simulate_failure) {
            logger << ">>> SIMULATED FAILURE MODE REQUESTED VIA CLI FLAG (--simulate-failure) <<<\n\n";
            record_failure("Deliberate validation failure simulated via --simulate-failure CLI option");
        }

        // Query system info
        SystemInfo sys = query_system_info();
        logger << "[SYSTEM HARDWARE & RUNTIME ENVIRONMENT]\n";
        logger << "  CPU Model:             " << sys.cpu_model << "\n";
        logger << "  Physical Cores:        " << sys.physical_cores << "\n";
        logger << "  Logical Processors:    " << sys.logical_processors << "\n";
        logger << "  Installed RAM:         " << std::fixed << std::setprecision(2) << sys.total_ram_gb << " GB\n";
        logger << "  Operating System:      " << sys.os_version << "\n";
        logger << "  Compiler:              " << sys.compiler_info << "\n";
        logger << "  Build Configuration:   " << sys.build_config << "\n";
        logger << "  Results Output Dir:    " << results_dir.string() << "\n\n";

        // Save benchmark environment text file safely
        {
            std::ostringstream env_oss;
            env_oss << "CPU Model: " << sys.cpu_model << "\n";
            env_oss << "Physical Cores: " << sys.physical_cores << "\n";
            env_oss << "Logical Processors: " << sys.logical_processors << "\n";
            env_oss << "RAM: " << std::fixed << std::setprecision(2) << sys.total_ram_gb << " GB\n";
            env_oss << "OS: " << sys.os_version << "\n";
            env_oss << "Compiler: " << sys.compiler_info << "\n";
            env_oss << "Build Configuration: " << sys.build_config << "\n";
            env_oss << "Multicore Seed: 42\n";
            env_oss << "Benchmark Case Count: 1000\n";
            env_oss << "Elements Per Case: 50\n";
            env_oss << "Conductivity Range: [5.0, 25.0] W/(m*K)\n";
            env_oss << "Heat Generation Range: [1000.0, 20000.0] W/m^3\n";
            env_oss << "Dirichlet Boundaries: T(0)=100.0 C, T(L)=20.0 C\n";
            env_oss << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";

            std::string err;
            if (!write_file_safely(logs_dir / "benchmark_environment.txt", env_oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            }
        }

        // ====================================================================
        // PART 1: 1D THERMAL FINITE ELEMENT ANALYSIS EXPERIMENTS
        // ====================================================================
        logger << "-------------------------------------------------------------------------------\n";
        logger << "1. EXECUTING 1D THERMAL FEA EXPERIMENTS\n";
        logger << "-------------------------------------------------------------------------------\n";
        
        workbench::domain::ThermalProblem base_problem;
        base_problem.length_m = 1.0;
        base_problem.area_m2 = 1e-4;
        base_problem.conductivity_W_mK = 10.0;
        base_problem.heat_generation_W_m3 = 10000.0;
        base_problem.left_temperature_C = 100.0;
        base_problem.right_temperature_C = 20.0;

        logger << "Base problem parameters:\n";
        logger << "  Length L: 1.0 m, Area A: 1e-4 m^2, k: 10.0 W/(m*K), q: 10000.0 W/m^3\n";
        logger << "  Dirichlet BCs: T(0) = 100.0 C, T(L) = 20.0 C\n\n";

        std::vector<int> mesh_counts = {10, 20, 40, 80};
        std::vector<workbench::domain::ThermalSolution> solutions;
        solutions.reserve(mesh_counts.size());

        for (int N : mesh_counts) {
            base_problem.element_count = N;
            auto sol = workbench::thermal::solve_thermal_1d(base_problem);
            solutions.push_back(sol);

            logger << "  [Mesh N = " << std::setw(2) << N << "] "
                   << "h = " << std::fixed << std::setprecision(4) << sol.element_size_h_m << " m | "
                   << "Max Nodal Err: " << std::scientific << std::setprecision(2) << sol.max_abs_nodal_error_C << " C | "
                   << "Max Midpoint Err: " << std::fixed << std::setprecision(6) << sol.max_abs_midpoint_error_C << " C | "
                   << "Theory Mid Err: " << std::fixed << std::setprecision(6) << sol.theoretical_max_midpoint_error_C << " C\n";
        }

        // 1. Export thermal_nodal_temperature_N10.csv safely
        const auto& sol_n10 = solutions.front();
        {
            std::ostringstream oss;
            oss << "node_index,x_m,T_fe_C,T_exact_C,abs_nodal_error_C\n";
            for (const auto& node : sol_n10.nodes) {
                oss << node.node_id << ","
                    << std::fixed << std::setprecision(4) << node.x_m << ","
                    << std::fixed << std::setprecision(6) << node.temperature_FE_C << ","
                    << std::fixed << std::setprecision(6) << node.temperature_exact_C << ","
                    << std::scientific << std::setprecision(2) << node.abs_error_C << "\n";
            }
            std::string err;
            if (!write_file_safely(results_dir / "thermal_nodal_temperature_N10.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "thermal_nodal_temperature_N10.csv").string() << "\n";
            }
        }

        // 2. Export thermal_midpoint_errors.csv safely
        {
            std::ostringstream oss;
            oss << "N_elements,element_index,x_mid_m,T_fe_mid_C,T_exact_mid_C,abs_midpoint_error_C\n";
            for (const auto& sol : solutions) {
                int N = sol.problem.element_count;
                for (const auto& mid : sol.midpoints) {
                    oss << N << ","
                        << mid.element_id << ","
                        << std::fixed << std::setprecision(4) << mid.x_mid_m << ","
                        << std::fixed << std::setprecision(6) << mid.temperature_FE_interp_C << ","
                        << std::fixed << std::setprecision(6) << mid.temperature_exact_C << ","
                        << std::fixed << std::setprecision(6) << mid.abs_error_C << "\n";
                }
            }
            std::string err;
            if (!write_file_safely(results_dir / "thermal_midpoint_errors.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "thermal_midpoint_errors.csv").string() << "\n";
            }
        }

        // 3. Export thermal_convergence_study.csv safely
        auto convergence_records = workbench::thermal::run_convergence_study(base_problem, mesh_counts);
        {
            std::ostringstream oss;
            oss << "N_elements,h_m,max_abs_midpoint_error_C,theoretical_midpoint_error_C,convergence_ratio\n";
            for (size_t i = 0; i < convergence_records.size(); ++i) {
                const auto& rec = convergence_records[i];
                std::string ratio_str = (i == 0) ? "N/A" : [&rec]() {
                    std::ostringstream ro;
                    ro << std::fixed << std::setprecision(4) << rec.error_ratio_vs_prior;
                    return ro.str();
                }();

                oss << rec.element_count_N << ","
                    << std::fixed << std::setprecision(4) << rec.element_size_h_m << ","
                    << std::fixed << std::setprecision(8) << rec.max_abs_midpoint_error_C << ","
                    << std::fixed << std::setprecision(8) << rec.theoretical_midpoint_error_C << ","
                    << ratio_str << "\n";
            }
            std::string err;
            if (!write_file_safely(results_dir / "thermal_convergence_study.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "thermal_convergence_study.csv").string() << "\n";
            }
        }

        // Verify Thermal Results with documented 1e-9 C threshold
        bool thermal_valid = true;
        for (const auto& sol : solutions) {
            if (sol.max_abs_nodal_error_C > 1e-9) {
                thermal_valid = false;
                record_failure("Thermal nodal error exceeded 1e-9 C threshold in mesh N=" + std::to_string(sol.problem.element_count));
                logger << "  [FAIL] Nodal error (" << sol.max_abs_nodal_error_C << " C) exceeded 1e-9 C in mesh N=" << sol.problem.element_count << "\n";
            }
            double mid_diff = std::abs(sol.max_abs_midpoint_error_C - sol.theoretical_max_midpoint_error_C);
            if (mid_diff > 1e-8) {
                thermal_valid = false;
                record_failure("Midpoint error discrepancy with theory exceeded 1e-8 C in mesh N=" + std::to_string(sol.problem.element_count));
                logger << "  [FAIL] Midpoint error discrepancy: " << mid_diff << "\n";
            }
        }
        for (size_t i = 1; i < convergence_records.size(); ++i) {
            double ratio_diff = std::abs(convergence_records[i].error_ratio_vs_prior - 4.0);
            if (ratio_diff > 1e-3) {
                thermal_valid = false;
                record_failure("Convergence ratio at mesh N=" + std::to_string(convergence_records[i].element_count_N) + " deviated from theoretical 4.0000");
                logger << "  [FAIL] Convergence ratio deviation: " << ratio_diff << "\n";
            }
        }
        if (thermal_valid) {
            logger << "  [PASS] Thermal FEA Verification: Exact nodal superconvergence (<= 1e-9 C) "
                   << "and exact quadratic midpoint error convergence (ratios ~ 4.0000).\n\n";
        }

        // ====================================================================
        // PART 2: MULTICORE PERFORMANCE BENCHMARK EXPERIMENTS
        // ====================================================================
        logger << "-------------------------------------------------------------------------------\n";
        logger << "2. EXECUTING MULTICORE BATCH BENCHMARK EXPERIMENTS (1,000 CASES)\n";
        logger << "-------------------------------------------------------------------------------\n";

        workbench::performance::BenchmarkConfig bench_cfg;
        bench_cfg.seed = 42;
        bench_cfg.case_count = 1000;
        bench_cfg.element_count = 50;
        bench_cfg.min_conductivity_W_mK = 5.0;
        bench_cfg.max_conductivity_W_mK = 25.0;
        bench_cfg.min_heat_generation_W_m3 = 1000.0;
        bench_cfg.max_heat_generation_W_m3 = 20000.0;
        bench_cfg.left_temperature_C = 100.0;
        bench_cfg.right_temperature_C = 20.0;
        bench_cfg.length_m = 1.0;
        bench_cfg.area_m2 = 1e-4;

        logger << "Generating " << bench_cfg.case_count << " deterministic cases (seed=" << bench_cfg.seed 
               << ", N=" << bench_cfg.element_count << ")...\n";
        auto cases = workbench::performance::generate_benchmark_cases(bench_cfg);
        logger << "Generated " << cases.size() << " cases successfully.\n\n";

        // Export benchmark_cases.csv safely
        {
            std::string err;
            if (!write_file_safely(results_dir / "benchmark_cases.csv", workbench::performance::format_benchmark_cases_csv(cases), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "benchmark_cases.csv").string() << "\n";
            }
        }

        std::vector<size_t> worker_counts = {1, 2, 3, 4};
        struct RunData {
            size_t p{1};
            double warmup_ms{0.0};
            std::vector<double> rep_times_ms;
            double median_ms{0.0};
            double mean_ms{0.0};
            double min_ms{0.0};
            double max_ms{0.0};
            double speedup{1.0};
            double efficiency{1.0};
            std::vector<workbench::domain::ThermalSolution> results;
        };

        std::vector<RunData> benchmark_data;
        benchmark_data.reserve(worker_counts.size());

        for (size_t p : worker_counts) {
            RunData rd;
            rd.p = p;
            logger << "Running Benchmark with Worker Count p = " << p << "...\n";

            // 1 unmeasured warm-up
            auto t_warm_start = std::chrono::steady_clock::now();
            if (p == 1) {
                workbench::performance::solve_batch_serial(cases);
            } else {
                workbench::performance::solve_batch_parallel(cases, p);
            }
            auto t_warm_end = std::chrono::steady_clock::now();
            rd.warmup_ms = std::chrono::duration<double, std::milli>(t_warm_end - t_warm_start).count();
            logger << "  Warm-up: " << std::fixed << std::setprecision(6) << rd.warmup_ms << " ms\n";

            // 5 measured repetitions
            rd.rep_times_ms.reserve(5);
            for (int r = 1; r <= 5; ++r) {
                auto t0 = std::chrono::steady_clock::now();
                if (p == 1) {
                    rd.results = workbench::performance::solve_batch_serial(cases);
                } else {
                    rd.results = workbench::performance::solve_batch_parallel(cases, p);
                }
                auto t1 = std::chrono::steady_clock::now();
                double el = std::chrono::duration<double, std::milli>(t1 - t0).count();
                rd.rep_times_ms.push_back(el);
                logger << "  Rep " << r << ":   " << std::fixed << std::setprecision(6) << el << " ms\n";
            }

            // Statistics (computed on exact full-precision values)
            std::vector<double> sorted = rd.rep_times_ms;
            std::sort(sorted.begin(), sorted.end());
            rd.median_ms = sorted[2]; // 3rd of 5 elements
            rd.min_ms = sorted.front();
            rd.max_ms = sorted.back();
            rd.mean_ms = std::accumulate(sorted.begin(), sorted.end(), 0.0) / 5.0;

            logger << "  -> Median: " << std::fixed << std::setprecision(6) << rd.median_ms << " ms (Min: "
                   << rd.min_ms << " ms, Max: " << rd.max_ms << " ms, Mean: " << rd.mean_ms << " ms)\n\n";

            benchmark_data.push_back(std::move(rd));
        }

        // Calculate speedup and efficiency relative to p = 1 median using exact unrounded values
        double t1_median = benchmark_data.front().median_ms;
        for (auto& rd : benchmark_data) {
            rd.speedup = t1_median / rd.median_ms;
            rd.efficiency = (rd.speedup / static_cast<double>(rd.p)) * 100.0;
        }

        // Export benchmark_raw_timings.csv with measurement_type, retaining at least 6 decimal places safely
        {
            std::ostringstream oss;
            oss << "workers_p,measurement_type,repetition,wall_clock_time_ms\n";
            for (const auto& rd : benchmark_data) {
                oss << rd.p << ",warmup,0," << std::fixed << std::setprecision(6) << rd.warmup_ms << "\n";
                for (size_t r = 0; r < rd.rep_times_ms.size(); ++r) {
                    oss << rd.p << ",measured," << (r + 1) << "," 
                        << std::fixed << std::setprecision(6) << rd.rep_times_ms[r] << "\n";
                }
            }
            std::string err;
            if (!write_file_safely(results_dir / "benchmark_raw_timings.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "benchmark_raw_timings.csv").string() << "\n";
            }
        }

        // Export benchmark_1000cases_summary.csv safely with mean, min, max
        {
            std::ostringstream oss;
            oss << "workers_p,warmup_ms,rep1_ms,rep2_ms,rep3_ms,rep4_ms,rep5_ms,median_ms,mean_ms,min_ms,max_ms,speedup_Sp,efficiency_Ep\n";
            for (const auto& rd : benchmark_data) {
                oss << rd.p << ","
                    << std::fixed << std::setprecision(6) << rd.warmup_ms << ","
                    << rd.rep_times_ms[0] << ","
                    << rd.rep_times_ms[1] << ","
                    << rd.rep_times_ms[2] << ","
                    << rd.rep_times_ms[3] << ","
                    << rd.rep_times_ms[4] << ","
                    << rd.median_ms << ","
                    << rd.mean_ms << ","
                    << rd.min_ms << ","
                    << rd.max_ms << ","
                    << std::fixed << std::setprecision(4) << rd.speedup << ","
                    << std::fixed << std::setprecision(2) << rd.efficiency << "%\n";
            }
            std::string err;
            if (!write_file_safely(results_dir / "benchmark_1000cases_summary.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "benchmark_1000cases_summary.csv").string() << "\n";
            }
        }

        // Serial vs Parallel Agreement Validation
        logger << "Verifying Serial vs Parallel Numerical Agreement (threshold = 1e-8 C)...\n";
        const auto& serial_results = benchmark_data.front().results;

        std::ostringstream agree_oss;
        agree_oss << "case_count,elements_per_case,serial_vs_parallel_workers,max_absolute_temp_diff_C,threshold_C,agreement_status\n";

        bool all_agreed = true;
        for (size_t i = 1; i < benchmark_data.size(); ++i) {
            const auto& par = benchmark_data[i];
            auto agr = workbench::performance::verify_agreement(serial_results, par.results, 1e-8);
            std::string label = "Serial vs Parallel (p=" + std::to_string(par.p) + ")";
            std::string status_str = agr.passed ? "PASS (Verified <= 1e-8 C)" : "FAIL";

            agree_oss << cases.size() << ","
                      << bench_cfg.element_count << ","
                      << label << ","
                      << std::scientific << std::setprecision(2) << agr.max_abs_difference_C << ","
                      << "1.00e-08" << ","
                      << status_str << "\n";

            logger << "  " << label << ": Max Diff = " << std::scientific << std::setprecision(2) 
                   << agr.max_abs_difference_C << " C | Status: " << status_str << "\n";

            if (!agr.passed || agr.max_abs_difference_C > 1e-8) {
                all_agreed = false;
                record_failure("Serial vs parallel agreement failure for worker count p=" + std::to_string(par.p));
            }
        }
        {
            std::string err;
            if (!write_file_safely(results_dir / "serial_parallel_agreement.csv", agree_oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "serial_parallel_agreement.csv").string() << "\n";
            }
        }
        if (all_agreed) {
            logger << "  [PASS] Benchmark Numerical Agreement Verified: Max diff <= 1e-8 C across all workers.\n\n";
        }

        // ====================================================================
        // PART 3: BACKPROPAGATION & FINITE DIFFERENCES EXPERIMENTS
        // ====================================================================
        logger << "-------------------------------------------------------------------------------\n";
        logger << "3. EXECUTING BACKPROPAGATION & GRADIENT VERIFICATION EXPERIMENTS\n";
        logger << "-------------------------------------------------------------------------------\n";

        workbench::backprop::Parameters init_params{0.5, 0.0, 1.0, 0.0};
        workbench::backprop::Model model(1.0, 2.0, init_params);

        logger << "Model configuration: Single-sample tanh regression\n";
        logger << "  Input x = 1.0, Target y = 2.0, Initial (w=0.5, b=0.0, v=1.0, c=0.0), lr = 0.1\n\n";

        // Initial forward pass
        auto fwd0 = model.forward();
        logger << "Initial Forward Pass:\n";
        logger << "  z = " << std::fixed << std::setprecision(6) << fwd0.z << "\n";
        logger << "  h = " << fwd0.h << "\n";
        logger << "  y_hat = " << fwd0.y_hat << "\n";
        logger << "  delta = " << fwd0.delta << "\n";
        logger << "  loss J = " << fwd0.loss << "\n\n";

        // Gradient Check
        logger << "Computing Analytical Gradients and Central Finite Differences (epsilon = 1e-6, threshold = 1e-5)...\n";
        auto grad_check = model.check_gradients(1e-6, 1e-5);
        if (!grad_check.passed) {
            record_failure("Backpropagation gradient check failed against 1e-5 threshold");
        }

        // Export backprop_gradient_check.csv safely
        {
            std::ostringstream oss;
            oss << "parameter,analytical_gradient,numerical_finite_diff,absolute_error,tolerance,status\n";
            auto write_row = [&](const std::string& name, double ana, double fd, double err) {
                std::string st = (err <= 1e-5) ? "PASS" : "FAIL";
                oss << name << ","
                    << std::fixed << std::setprecision(8) << ana << ","
                    << std::fixed << std::setprecision(8) << fd << ","
                    << std::scientific << std::setprecision(2) << err << ","
                    << "1e-5" << ","
                    << st << "\n";
                logger << "  Param " << name << ": Ana = " << std::fixed << std::setprecision(8) << ana
                       << " | FD = " << fd << " | Err = " << std::scientific << std::setprecision(2) << err 
                       << " | Status: " << st << "\n";
            };
            write_row("w", grad_check.analytical.dw, grad_check.finite_difference.dw, grad_check.absolute_error.dw);
            write_row("b", grad_check.analytical.db, grad_check.finite_difference.db, grad_check.absolute_error.db);
            write_row("v", grad_check.analytical.dv, grad_check.finite_difference.dv, grad_check.absolute_error.dv);
            write_row("c", grad_check.analytical.dc, grad_check.finite_difference.dc, grad_check.absolute_error.dc);

            std::string err;
            if (!write_file_safely(results_dir / "backprop_gradient_check.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "backprop_gradient_check.csv").string() << "\n";
            }
        }
        if (grad_check.passed) {
            logger << "  [PASS] Gradient Check Passed: All parameter gradients match finite differences to within <= 1e-5.\n\n";
        }

        // 100-step Training Trace
        logger << "Executing 100-step Simultaneous Gradient Descent Training Trace (lr = 0.1)...\n";
        auto trace = model.train(100, 0.1);

        // Export backprop_training_trace_100steps.csv safely
        {
            std::ostringstream oss;
            oss << "step,loss_J,y_hat,w,b,v,c,delta\n";
            for (const auto& rec : trace) {
                double delta = rec.y_hat - 2.0;
                oss << rec.step << ","
                    << std::scientific << std::setprecision(8) << rec.loss << ","
                    << std::fixed << std::setprecision(8) << rec.y_hat << ","
                    << std::fixed << std::setprecision(6) << rec.parameters.w << ","
                    << std::fixed << std::setprecision(6) << rec.parameters.b << ","
                    << std::fixed << std::setprecision(6) << rec.parameters.v << ","
                    << std::fixed << std::setprecision(6) << rec.parameters.c << ","
                    << std::fixed << std::setprecision(6) << delta << "\n";
            }
            std::string err;
            if (!write_file_safely(results_dir / "backprop_training_trace_100steps.csv", oss.str(), err)) {
                record_failure(err);
                logger << "  [FAIL] " << err << "\n";
            } else {
                logger << "  -> Exported: " << (results_dir / "backprop_training_trace_100steps.csv").string() << "\n";
            }
        }

        // Complete trajectory validation across all 101 records
        bool trace_valid = true;
        std::string trace_error_reason;
        if (trace.size() != 101) {
            trace_valid = false;
            trace_error_reason = "Expected 101 trace records, but received " + std::to_string(trace.size());
        } else {
            for (size_t s = 0; s < trace.size(); ++s) {
                const auto& rec = trace[s];
                if (!std::isfinite(rec.loss) || !std::isfinite(rec.y_hat) ||
                    !std::isfinite(rec.parameters.w) || !std::isfinite(rec.parameters.b) ||
                    !std::isfinite(rec.parameters.v) || !std::isfinite(rec.parameters.c)) {
                    trace_valid = false;
                    trace_error_reason = "Non-finite numeric value encountered at step " + std::to_string(s);
                    break;
                }
                if (s > 0) {
                    if (!(rec.loss < trace[s - 1].loss)) {
                        trace_valid = false;
                        trace_error_reason = "Loss at step " + std::to_string(s) + " (" + 
                                             std::to_string(rec.loss) + ") is not strictly lower than step " + 
                                             std::to_string(s - 1) + " (" + std::to_string(trace[s - 1].loss) + ")";
                        break;
                    }
                }
            }
        }

        if (!trace_valid) {
            record_failure("Backpropagation training trace validation failure: " + trace_error_reason);
            logger << "  [FAIL] Training Trace Validation Failed: " << trace_error_reason << "\n\n";
        } else {
            logger << "  [PASS] Training Trace Verified: 101 consecutive steps are strictly monotonically decreasing and fully finite.\n\n";
        }

        logger << "  Trace summary milestones:\n";
        logger << "    Step   0: Loss = " << std::scientific << std::setprecision(6) << trace[0].loss 
               << ", y_hat = " << std::fixed << std::setprecision(6) << trace[0].y_hat << "\n";
        logger << "    Step   1: Loss = " << std::scientific << std::setprecision(6) << trace[1].loss 
               << ", y_hat = " << std::fixed << std::setprecision(6) << trace[1].y_hat << "\n";
        logger << "    Step  10: Loss = " << std::scientific << std::setprecision(6) << trace[10].loss 
               << ", y_hat = " << std::fixed << std::setprecision(6) << trace[10].y_hat << "\n";
        logger << "    Step  50: Loss = " << std::scientific << std::setprecision(6) << trace[50].loss 
               << ", y_hat = " << std::fixed << std::setprecision(6) << trace[50].y_hat << "\n";
        logger << "    Step 100: Loss = " << std::scientific << std::setprecision(6) << trace[100].loss 
               << ", y_hat = " << std::fixed << std::setprecision(6) << trace[100].y_hat << "\n\n";

        logger << "===============================================================================\n";
        if (all_validations_passed) {
            logger << "  EXPERIMENTAL RUN STATUS: [PASS] All criteria verified successfully!\n";
            logger << "===============================================================================\n";
        } else {
            logger << "  EXPERIMENTAL RUN STATUS: [FAIL] Validation failures detected:\n";
            for (const auto& f_msg : validation_failures) {
                logger << "    * " << f_msg << "\n";
            }
            logger << "===============================================================================\n";
        }

        // Explicitly close execution log and verify status before exiting
        bool log_closed_cleanly = logger.close();
        if (!log_closed_cleanly) {
            std::cerr << "FATAL ERROR: Failed to cleanly flush and close experiment execution log at: "
                      << (logs_dir / "experiment_execution.log").string() << "\n";
            return 1;
        }

        if (all_validations_passed) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "FATAL ERROR during experimental execution: " << ex.what() << "\n";
        return 1;
    }
}
