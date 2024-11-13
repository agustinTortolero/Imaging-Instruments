#ifndef VECTOR_FILTERING_PLUGIN_H
#define VECTOR_FILTERING_PLUGIN_H

#include "vector_filtering_global.h"  // Include the global header
#include "plugin_interface_filtering.h"
#include "vector_filtering.h"  // The actual filtering logic header

class VECTORFILTERING_EXPORT VectorFiltering : public QObject, public PluginInterfaceFiltering
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGININTERFACE_IID)  // This is a unique identifier for your plugin
    Q_INTERFACES(PluginInterfaceFiltering)  // Define the interface your plugin implements

public:
    VectorFiltering();  // Constructor
    ~VectorFiltering();  // Destructor

    QString context_menu_str();  // Function to return the context menu string
    void processImage(const cv::Mat &inputImage1, cv::Mat &outputImage);  // Single image processing
    void processImage(const cv::Mat &inputImage1, const cv::Mat &inputImage2, cv::Mat &outputImage);  // Dual image processing
};

#endif // VECTOR_FILTERING_PLUGIN_H
