#ifndef PLUGIN_INTERFACE_NOISE_H
#define PLUGIN_INTERFACE_NOISE_H

#include <QObject>
#include <QString>
#include "opencv2/opencv.hpp"

// Ensure PluginInterfaceNoise has the Q_OBJECT macro if using signals/slots in Qt
class PluginInterfaceNoise
{
public:
    virtual ~PluginInterfaceNoise() {}
    virtual QString context_menu_str() = 0;
    virtual void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const float noise_density) = 0;
};

#define PLUGININTERFACE_IID "agustin.tortolero"
Q_DECLARE_INTERFACE(PluginInterfaceNoise, PLUGININTERFACE_IID)

#endif // PLUGIN_INTERFACE_NOISE_H
