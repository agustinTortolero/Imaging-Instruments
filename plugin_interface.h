#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QObject>
#include <QString>
#include <QMap>
#include "opencv2/opencv.hpp"

// Plugin Interface for Color Enhancement
class PluginInterfaceColorEnhancement
{
public:
    virtual ~PluginInterfaceColorEnhancement() {}
    virtual void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const std::string &parameter) = 0;
};

// Plugin Interface for Filtering
class PluginInterfaceFiltering
{
public:
    virtual ~PluginInterfaceFiltering() {}
    virtual QString context_menu_str() = 0;
    virtual void processImage(const cv::Mat &inputImage1, cv::Mat &outputImage) = 0; // For grayscale and color images
    virtual void processImage(const cv::Mat &inputImage1, const cv::Mat &inputImage2, cv::Mat &outputImage) = 0; // For video processing (2D)
};

// Plugin Interface for Noise
class PluginInterfaceNoise
{
public:
    virtual ~PluginInterfaceNoise() {}
    virtual QString context_menu_str() = 0;
    virtual void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const float noise_density) = 0;
};

class PluginInstrument
{
public:
    virtual ~PluginInstrument() {}

    virtual void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const QMap<QString, QVariant> &parameters = {}) = 0;
    virtual QString context_menu_str() const = 0;
    virtual QString plugin_name() const = 0;
    virtual QString plugin_version() const = 0;
    virtual QString compatible_app_version() const = 0;
    virtual QMap<QString, QVariant> defaultParameters() const = 0;

};


// Single Plugin Interface ID for all plugins
#define PLUGININTERFACE_IID "agustin.tortolero"
Q_DECLARE_INTERFACE(PluginInterfaceColorEnhancement, PLUGININTERFACE_IID)
Q_DECLARE_INTERFACE(PluginInterfaceFiltering, PLUGININTERFACE_IID)
Q_DECLARE_INTERFACE(PluginInterfaceNoise, PLUGININTERFACE_IID)
Q_DECLARE_INTERFACE(PluginInstrument, PLUGININTERFACE_IID)
#endif // PLUGIN_INTERFACE_H
