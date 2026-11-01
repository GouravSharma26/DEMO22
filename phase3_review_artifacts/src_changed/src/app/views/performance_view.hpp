#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>
#include <atomic>
#include <thread>
#include "workbench/performance/benchmark.hpp"
#include "widgets/plot_widget.hpp"

namespace workbench {
namespace app {

class PerformanceView : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceView(QWidget* parent = nullptr);
    ~PerformanceView() override;

    bool hasSuiteResult() const noexcept { return current_suite_.has_value(); }
    const workbench::performance::BenchmarkSuiteResult& currentSuite() const { return *current_suite_; }

    void setCaseCount(int cases) { if (cases_spin_) cases_spin_->setValue(cases); }
    void setElementCount(int elements) { if (elements_spin_) elements_spin_->setValue(elements); }
    bool isRunButtonEnabled() const { return run_btn_ && run_btn_->isEnabled(); }
    bool isCancelButtonEnabled() const { return cancel_btn_ && cancel_btn_->isEnabled(); }
    QString statusText() const { return status_label_ ? status_label_->text() : QString(); }

public slots:
    void onRunBenchmarkClicked();
    void onCancelClicked();

signals:
    void benchmarkProgress(size_t worker_count, int repetition);
    void benchmarkCompleted(const workbench::performance::BenchmarkSuiteResult& result);
    void benchmarkCancelled();
    void benchmarkFailed(const QString& error);

private slots:
    void onBenchmarkCompleted(const workbench::performance::BenchmarkSuiteResult& result);
    void onBenchmarkCancelled();
    void onBenchmarkFailed(const QString& error);
    void onExportCasesClicked();
    void onExportRawTimingsClicked();
    void onExportSummaryClicked();
    void onExportAgreementClicked();

private:
    void setupUi();
    void updatePlots(const workbench::performance::BenchmarkSuiteResult& suite);

    // Inputs
    QSpinBox* seed_spin_{nullptr};
    QSpinBox* cases_spin_{nullptr};
    QSpinBox* elements_spin_{nullptr};
    QCheckBox* worker_1_check_{nullptr};
    QCheckBox* worker_2_check_{nullptr};
    QCheckBox* worker_4_check_{nullptr};

    // Buttons & Progress
    QPushButton* run_btn_{nullptr};
    QPushButton* cancel_btn_{nullptr};
    QProgressBar* progress_bar_{nullptr};
    QLabel* status_label_{nullptr};
    QLabel* agreement_label_{nullptr};

    // Tables & Plot
    QTableWidget* summary_table_{nullptr};
    QTableWidget* raw_table_{nullptr};
    PlotWidget* speedup_plot_{nullptr};

    // Background Execution
    std::atomic<bool> cancel_flag_{false};
    std::thread worker_thread_;

    // Cached outputs for export
    std::vector<domain::ThermalProblem> current_cases_;
    std::optional<workbench::performance::BenchmarkSuiteResult> current_suite_;
};

} // namespace app
} // namespace workbench

Q_DECLARE_METATYPE(workbench::performance::BenchmarkSuiteResult)

