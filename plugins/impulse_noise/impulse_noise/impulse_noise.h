#ifndef IMPULSE_NOISE_PLUGIN_H
#define IMPULSE_NOISE_PLUGIN_H

#include "impulse_noise_global.h"
#include "cvplugininterface.h"

class IMPULSE_NOISE_EXPORT Impulse_noise : public QObject, public CvPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CVPLUGININTERFACE_IID)
    Q_INTERFACES(CvPluginInterface)

public:
    Impulse_noise();
    ~Impulse_noise();
    QString description() override;
    void processImage(const cv::Mat &inputImage, cv::Mat &outputImage) override;
private:
    void addSaltAndPepperNoise(cv::Mat &image, float saltPepperRatio = 0.5);
};

#endif


