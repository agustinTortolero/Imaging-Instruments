#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H


#include <QObject>
#include <QString>
#include "opencv2/opencv.hpp"

class PluginInterfaceColorEnhancement
{
public:
    virtual ~PluginInterfaceColorEnhancement() {}
    virtual void processImage(const cv::Mat &, cv::Mat &, const std::string& ) = 0;
};

#define PLUGININTERFACE_IID "agustin.tortolero"
Q_DECLARE_INTERFACE(PluginInterfaceColorEnhancement, PLUGININTERFACE_IID)

#endif // CVPLUGININTERFACE_H
