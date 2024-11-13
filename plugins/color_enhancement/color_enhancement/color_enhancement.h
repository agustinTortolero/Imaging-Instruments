#ifndef COLOR_ENHANCEMENT_H
#define COLOR_ENHANCEMENT_H

#include "color_enhancement_global.h"
#include "plugin_interface.h"

class COLOR_ENHANCEMENT_EXPORT ColorEnhancement : public QObject, public PluginInterfaceColorEnhancement {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGININTERFACE_IID)
    Q_INTERFACES(PluginInterfaceColorEnhancement)

public:
    ColorEnhancement() = default;
    ~ColorEnhancement() = default;

    void processImage(const cv::Mat &image, cv::Mat &outputImage, const std::string &method) override;

private:
    static cv::Mat histogramEqualization(const cv::Mat& img);
    static cv::Mat adaptiveHistogramEqualization(const cv::Mat& cv2Img);
    static cv::Mat claheEqualization(const cv::Mat& cv2Img);

    static void calculateHistograms(const cv::Mat& img, std::vector<cv::Mat>& hist);

};

#endif // COLOR_ENHANCEMENT_H
