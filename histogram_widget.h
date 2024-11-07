#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <opencv2/opencv.hpp>
#include <QtCharts/QChart>

class HistogramWidget : public QWidget {
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget* parent = nullptr);

    void updateHistogram(const cv::Mat& image); // Method to update the histogram with a new image

private:
    void calculateHistogram(const cv::Mat& image, std::vector<cv::Mat>& histograms);
    QLineSeries* createSeries(const cv::Mat& hist, const QColor& color);

    QChart* chart; // Chart for displaying the histogram
    QChartView* chartView; // View for the chart
};

#endif // HISTOGRAMWIDGET_H
