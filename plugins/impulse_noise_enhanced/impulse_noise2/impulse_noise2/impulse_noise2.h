#ifndef IMPULSE_NOISE2_H
#define IMPULSE_NOISE2_H

#include "impulse_noise2_global.h"
#include "plugin_interface_noise.h"
#include <QObject>
#include <QString>
#include <opencv2/core.hpp>

enum class NoiseType {
    SALT_AND_PEPPER,
    RANDOM_VALUE
};

// Ensure the class uses the export/import macro
class IMPULSE_NOISE2_EXPORT ImpulseNoise : public QObject, public PluginInterfaceNoise
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGININTERFACE_IID)
    Q_INTERFACES(PluginInterfaceNoise)

public:
    ImpulseNoise();
    ~ImpulseNoise();
    QString context_menu_str() override;
    void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const float noise_density) override;

private:
    void addSaltAndPepperNoise(cv::Mat &image, const float noise_density);
};

#endif // IMPULSE_NOISE2_H


