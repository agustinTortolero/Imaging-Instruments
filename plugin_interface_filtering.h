#ifndef PLUGIN_INTERFACE_FILTERING_H
#define PLUGIN_INTERFACE_FILTERING_H


#include <QObject>
#include <QString>
#include "opencv2/opencv.hpp"

class PluginInterfaceFiltering
{
public:
    virtual ~PluginInterfaceFiltering() {}
    virtual QString context_menu_str() = 0;
    virtual void processImage(const cv::Mat &inputImage1, cv::Mat &outputImage) = 0;//for grayscale and color images
    virtual void processImage(const cv::Mat &inputImage1, const cv::Mat &inputImage2, cv::Mat &outputImage) = 0; // for video procesing (2D)
};

#define PLUGININTERFACE_IID "agustin.tortolero"
Q_DECLARE_INTERFACE(PluginInterfaceFiltering, PLUGININTERFACE_IID)



#endif // PLUGIN_INTERFACE_FILTERING_H
