#include "histogram_widget.h"

HistogramWidget::HistogramWidget(QWidget* parent)
    : QWidget(parent), chart(new QChart()), chartView(new QChartView(chart, this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
}

void HistogramWidget::updateHistogram(const cv::Mat& image) {
    std::vector<cv::Mat> histograms;
    calculateHistogram(image, histograms);
    chart->removeAllSeries(); // Clear previous series

    // Create series for each channel
    const QColor colors[] = { Qt::red, Qt::green, Qt::blue };
    for (size_t i = 0; i < histograms.size(); ++i) {
        chart->addSeries(createSeries(histograms[i], colors[i]));
    }

    chart->createDefaultAxes(); // Create default axes
    chartView->setRenderHint(QPainter::Antialiasing); // Enable anti-aliasing
}

void HistogramWidget::calculateHistogram(const cv::Mat& image, std::vector<cv::Mat>& histograms) {
    // Assuming image is in BGR format, split into channels
    cv::Mat bgrChannels[3];
    cv::split(image, bgrChannels);

    histograms.resize(3);
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    for (int i = 0; i < 3; ++i) {
        cv::calcHist(&bgrChannels[i], 1, 0, cv::Mat(), histograms[i], 1, &histSize, &histRange);
    }
}

QLineSeries* HistogramWidget::createSeries(const cv::Mat& hist, const QColor& color) {
    QLineSeries* series = new QLineSeries();
    for (int i = 0; i < hist.rows; ++i) {
        series->append(i, hist.at<float>(i));
    }
    series->setColor(color);
    return series;
}
