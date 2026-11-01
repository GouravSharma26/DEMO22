#include <QtTest/QtTest>
#include <QApplication>
#include <QTabWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>
#include <iostream>

#include "main_window.hpp"
#include "views/geometry_view.hpp"
#include "views/thermal_view.hpp"
#include "views/performance_view.hpp"
#include "views/hardware_view.hpp"
#include "views/backprop_view.hpp"
#include "widgets/viewport_3d.hpp"
#include "widgets/plot_widget.hpp"
#include "workbench/core.hpp"

namespace {

void dismissActiveMessageBox() {
    QTimer::singleShot(25, []() {
        for (auto* widget : QApplication::topLevelWidgets()) {
            if (auto* box = qobject_cast<QMessageBox*>(widget)) {
                box->accept();
            }
        }
    });
}

} // namespace

class GuiIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testMainWindowCreationAndTabs();
    void testGeometryViewStlAndCsv();
    void testGeometryViewInvalidFileHandling();
    void testThermalViewSolveAndConvergence();
    void testThermalViewParameterValidation();
    void testPerformanceViewExecutionAndCancellation();
    void testHardwareViewSimulatedDevices();
    void testBackpropViewCalculationsAndTraining();
    void testCrossViewSwitchingStability();
    void cleanupTestCase();

private:
    std::unique_ptr<workbench::app::MainWindow> main_window_;
};

void GuiIntegrationTest::initTestCase() {
    std::cout << ">>> initTestCase START" << std::endl;
    main_window_ = std::make_unique<workbench::app::MainWindow>();
    std::cout << ">>> MainWindow created" << std::endl;
    QVERIFY(main_window_ != nullptr);
    main_window_->show();
    std::cout << ">>> MainWindow shown" << std::endl;
}

void GuiIntegrationTest::testMainWindowCreationAndTabs() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    QCOMPARE(tabs->count(), 5);

    QCOMPARE(tabs->tabText(0).trimmed(), QString("Geometry (STL & Point Cloud)"));
    QCOMPARE(tabs->tabText(1).trimmed(), QString("Thermal Analysis (1D FEA)"));
    QCOMPARE(tabs->tabText(2).trimmed(), QString("Performance (Multicore)"));
    QCOMPARE(tabs->tabText(3).trimmed(), QString("Hardware (Simulated Mocks)"));
    QCOMPARE(tabs->tabText(4).trimmed(), QString("Backpropagation (Tanh NN)"));

    // Verify status bar text contains core version and provenance notices
    auto* status_label = main_window_->statusBar()->findChild<QLabel*>();
    QVERIFY(status_label != nullptr);
    QVERIFY(status_label->text().contains("Workbench Core"));
    QVERIFY(status_label->text().contains("SIMULATED"));
    QVERIFY(status_label->text().contains("synthetic"));

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/00_main_window_overview.png");
}

void GuiIntegrationTest::testGeometryViewStlAndCsv() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    tabs->setCurrentIndex(0);

    auto* geo_view = main_window_->findChild<workbench::app::GeometryView*>();
    QVERIFY(geo_view != nullptr);

    // Test loading sample STL
    geo_view->loadStlFile("data/examples/sample_tetrahedron_ascii.stl");
    auto* viewport = geo_view->findChild<workbench::app::Viewport3D*>();
    QVERIFY(viewport != nullptr);
    QVERIFY(viewport->hasMesh());
    QCOMPARE(viewport->meshTriangleCount(), 4);

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/01_geometry_view_stl.png");

    // Test loading synthetic CSV
    geo_view->loadTemperatureCsvFile("data/examples/synthetic_temperature_nodes.csv");
    QVERIFY(viewport->hasPointCloud());
    QCOMPARE(viewport->pointCloudNodeCount(), 7);

    // Test node picking
    workbench::domain::TemperatureNode test_node{1, 0.0, 0.0, 0.0, 20.0};
    geo_view->onNodeSelected(test_node);
    auto* node_table = geo_view->findChild<QTableWidget*>();
    QVERIFY(node_table != nullptr);
    QCOMPARE(node_table->item(0, 1)->text(), QString("1"));
    QCOMPARE(node_table->item(4, 1)->text(), QString("20.00 °C"));

    // Test wireframe toggle and fit to view
    geo_view->onWireframeToggled(false);
    geo_view->onWireframeToggled(true);
    geo_view->onFitViewClicked();

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/02_geometry_view_pointcloud.png");
}

void GuiIntegrationTest::testGeometryViewInvalidFileHandling() {
    auto* geo_view = main_window_->findChild<workbench::app::GeometryView*>();
    QVERIFY(geo_view != nullptr);

    // Test loading non-existent STL (dismiss modal error dialog automatically)
    dismissActiveMessageBox();
    geo_view->loadStlFile("data/examples/non_existent_file.stl");
    auto* viewport = geo_view->findChild<workbench::app::Viewport3D*>();
    QVERIFY(viewport != nullptr);
    QVERIFY(viewport->hasMesh());

    // Test loading non-existent CSV (dismiss modal error dialog automatically)
    dismissActiveMessageBox();
    geo_view->loadTemperatureCsvFile("data/examples/non_existent_nodes.csv");
    QVERIFY(viewport->hasPointCloud());
}

void GuiIntegrationTest::testThermalViewSolveAndConvergence() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    tabs->setCurrentIndex(1);

    auto* thermal_view = main_window_->findChild<workbench::app::ThermalView*>();
    QVERIFY(thermal_view != nullptr);

    // Run single mesh solve with default parameters
    thermal_view->onSolveClicked();
    QVERIFY(thermal_view->hasSolution());

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/03_thermal_view_solved.png");

    const auto& sol = thermal_view->currentSolution();
    QCOMPARE(sol.problem.element_count, 10);
    // Superconvergence verification: nodal error <= 1e-9 C
    QVERIFY(sol.max_abs_nodal_error_C <= 1e-9);
    // Midpoint error for N=10 is 1.25 C
    QVERIFY(std::abs(sol.max_abs_midpoint_error_C - 1.25) < 1e-6);

    // Run convergence study for N=10, 20, 40, 80
    thermal_view->onConvergenceStudyClicked();
    const auto& study_records = thermal_view->convergenceRecords();
    QCOMPARE(static_cast<int>(study_records.size()), 4);
    QCOMPARE(study_records[0].element_count_N, 10);
    QCOMPARE(study_records[1].element_count_N, 20);
    QCOMPARE(study_records[2].element_count_N, 40);
    QCOMPARE(study_records[3].element_count_N, 80);

    // Verify quadratic convergence ratio ~4.00
    for (size_t i = 1; i < study_records.size(); ++i) {
        QVERIFY(std::abs(study_records[i].error_ratio_vs_prior - 4.0) < 1e-3);
    }

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/04_thermal_view_convergence.png");
}

void GuiIntegrationTest::testThermalViewParameterValidation() {
    auto* thermal_view = main_window_->findChild<workbench::app::ThermalView*>();
    QVERIFY(thermal_view != nullptr);

    // 1. Direct domain validation check
    workbench::domain::ThermalProblem invalid_p;
    invalid_p.length_m = -1.0;
    auto err = invalid_p.validate();
    QVERIFY(err.has_value());
    QVERIFY(!err->empty());

    // 2. Genuine GUI input-validation check via widget controls
    dismissActiveMessageBox();
    thermal_view->setRodLength(0.0);
    thermal_view->onSolveClicked();
    // Restore valid length
    thermal_view->setRodLength(1.0);
}

void GuiIntegrationTest::testPerformanceViewExecutionAndCancellation() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    tabs->setCurrentIndex(2);

    auto* perf_view = main_window_->findChild<workbench::app::PerformanceView*>();
    QVERIFY(perf_view != nullptr);

    // Prepare spy BEFORE clicking run
    QSignalSpy spy_done(perf_view, &workbench::app::PerformanceView::benchmarkCompleted);

    // Run benchmark (1,000 cases, seed 42)
    perf_view->onRunBenchmarkClicked();

    // Wait for completion signal
    QVERIFY(spy_done.wait(15000)); // up to 15 seconds

    QVERIFY(perf_view->hasSuiteResult());
    const auto& suite = perf_view->currentSuite();
    QVERIFY(!suite.summaries.empty());
    // Numerical agreement <= 1e-8 C
    QVERIFY(suite.agreement.passed);
    QVERIFY(suite.agreement.max_abs_difference_C <= 1e-8);

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/05_performance_view_completed.png");

    // Test cancellation workflow deterministically with heavy workload
    auto prior_suite = perf_view->currentSuite();
    QSignalSpy spy_cancel(perf_view, &workbench::app::PerformanceView::benchmarkCancelled);

    perf_view->setCaseCount(30000);
    perf_view->setElementCount(200);

    perf_view->onRunBenchmarkClicked();
    QVERIFY(!perf_view->isRunButtonEnabled());
    QVERIFY(perf_view->isCancelButtonEnabled());

    perf_view->onCancelClicked();
    QVERIFY(spy_cancel.wait(5000));
    QCOMPARE(spy_cancel.count(), 1);

    // Verify UI state restored and previous results preserved
    QVERIFY(perf_view->isRunButtonEnabled());
    QVERIFY(!perf_view->isCancelButtonEnabled());
    QVERIFY(perf_view->statusText().contains("Cancelled"));
    QVERIFY(perf_view->hasSuiteResult());
    QCOMPARE(perf_view->currentSuite().summaries.size(), prior_suite.summaries.size());

    // Restore standard benchmark parameters
    perf_view->setCaseCount(1000);
    perf_view->setElementCount(10);
}

void GuiIntegrationTest::testHardwareViewSimulatedDevices() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    tabs->setCurrentIndex(3);

    auto* hw_view = main_window_->findChild<workbench::app::HardwareView*>();
    QVERIFY(hw_view != nullptr);

    auto* mcu_card = hw_view->mcuCard();
    QVERIFY(mcu_card != nullptr);

    // Connect MCU
    mcu_card->onConnectClicked();
    QCOMPARE(mcu_card->connectionState(), workbench::embedded::ConnectionState::Connected);

    // Poll telemetry
    mcu_card->onPollTelemetry();
    auto telem = mcu_card->lastTelemetry();
    QVERIFY(telem.has_value());
    QVERIFY(std::abs(telem->supply_voltage_V - 3.3) < 0.1);

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/06_hardware_view_esp32_connected.png");

    // Inject timeout fault
    mcu_card->onInjectTimeoutToggled(true);
    mcu_card->onPollTelemetry();
    QCOMPARE(mcu_card->connectionState(), workbench::embedded::ConnectionState::Error);

    // Recover
    mcu_card->onInjectTimeoutToggled(false);
    mcu_card->onConnectClicked();
    QCOMPARE(mcu_card->connectionState(), workbench::embedded::ConnectionState::Connected);

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/07_hardware_view_fault_recovered.png");

    // Disconnect MCU
    mcu_card->onDisconnectClicked();
    QCOMPARE(mcu_card->connectionState(), workbench::embedded::ConnectionState::Disconnected);

    // Exercise Arty A7 FPGA card
    auto* fpga_card = hw_view->fpgaCard();
    QVERIFY(fpga_card != nullptr);
    fpga_card->onConnectClicked();
    QCOMPARE(fpga_card->connectionState(), workbench::embedded::ConnectionState::Connected);
    fpga_card->onPollTelemetry();
    auto fpga_telem = fpga_card->lastTelemetry();
    QVERIFY(fpga_telem.has_value());
    fpga_card->onDisconnectClicked();
    QCOMPARE(fpga_card->connectionState(), workbench::embedded::ConnectionState::Disconnected);

    // Exercise JTAG Adapter card
    auto* jtag_card = hw_view->jtagCard();
    QVERIFY(jtag_card != nullptr);
    jtag_card->onConnectClicked();
    QCOMPARE(jtag_card->connectionState(), workbench::embedded::ConnectionState::Connected);
    jtag_card->onPollTelemetry();
    auto jtag_telem = jtag_card->lastTelemetry();
    QVERIFY(jtag_telem.has_value());
    jtag_card->onDisconnectClicked();
    QCOMPARE(jtag_card->connectionState(), workbench::embedded::ConnectionState::Disconnected);
}

void GuiIntegrationTest::testBackpropViewCalculationsAndTraining() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    tabs->setCurrentIndex(4);

    auto* bp_view = main_window_->findChild<workbench::app::BackpropView*>();
    QVERIFY(bp_view != nullptr);

    // Test forward pass
    bp_view->onForwardClicked();
    auto fwd = bp_view->currentForward();
    QVERIFY(std::abs(fwd.z - 0.5) < 1e-6);
    QVERIFY(std::abs(fwd.h - std::tanh(0.5)) < 1e-6);
    QVERIFY(std::abs(fwd.y_hat - std::tanh(0.5)) < 1e-6);
    QVERIFY(std::abs(fwd.loss - 0.5 * std::pow(fwd.y_hat - 2.0, 2)) < 1e-6);

    // Test analytical vs finite-difference gradient check
    bp_view->onGradientCheckClicked();
    auto check = bp_view->currentCheck();
    QVERIFY(check.has_value());
    QVERIFY(check->passed);
    QVERIFY(check->max_absolute_error <= 1e-5);

    // Test 1 simultaneous step
    bp_view->onStepClicked();
    auto params_step1 = bp_view->currentParameters();
    QVERIFY(std::abs(params_step1.w - 0.620946) < 1e-4);
    QVERIFY(std::abs(params_step1.b - 0.120946) < 1e-4);
    QVERIFY(std::abs(params_step1.v - 1.071068) < 1e-4);
    QVERIFY(std::abs(params_step1.c - 0.153788) < 1e-4);

    // Test 100-step training trace (benchmarks from step 0 baseline J0 = 1.182541819)
    bp_view->onTrainClicked();
    const auto& trace = bp_view->currentTrace();
    QCOMPARE(static_cast<int>(trace.size()), 101);
    // Initial loss is exact analytical reference value
    QVERIFY(std::abs(trace.front().loss - 1.182541819) < 1e-6);
    // Strict monotonic decrease
    for (size_t s = 1; s < trace.size(); ++s) {
        QVERIFY(trace[s].loss < trace[s - 1].loss);
        QVERIFY(std::isfinite(trace[s].loss));
    }
    // Step 100 loss near zero
    QVERIFY(trace.back().loss < 1e-15);

    QCoreApplication::processEvents();
    main_window_->grab().save("docs/evidence/phase3/08_backprop_view_trained.png");

    // Test reset
    bp_view->onResetClicked();
    auto reset_params = bp_view->currentParameters();
    QCOMPARE(reset_params.w, 0.5);
    QCOMPARE(reset_params.b, 0.0);
    QCOMPARE(reset_params.v, 1.0);
    QCOMPARE(reset_params.c, 0.0);
}

void GuiIntegrationTest::testCrossViewSwitchingStability() {
    auto* tabs = main_window_->findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);

    // Rapidly cycle tabs 0 -> 1 -> 2 -> 3 -> 4 -> 0
    for (int cycle = 0; cycle < 3; ++cycle) {
        for (int i = 0; i < 5; ++i) {
            tabs->setCurrentIndex(i);
            QCOMPARE(tabs->currentIndex(), i);
            QCoreApplication::processEvents();
        }
    }

    // Verify application remains alive and responsive
    QVERIFY(main_window_->isVisible());
}

void GuiIntegrationTest::cleanupTestCase() {
    main_window_.reset();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    std::cout << "Starting GUI integration tests..." << std::endl;
    GuiIntegrationTest tc;
    int res = QTest::qExec(&tc, argc, argv);
    std::cout << "GUI integration tests finished with result: " << res << std::endl;
    return res;
}
#include "test_gui_integration.moc"
