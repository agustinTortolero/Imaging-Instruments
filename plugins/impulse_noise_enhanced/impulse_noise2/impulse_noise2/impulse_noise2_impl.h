#ifndef IMPULSE_NOISE2_IMPL_H
#define IMPULSE_NOISE2_IMPL_H

#include "impulse_noise2.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <cstdlib>
#include <ctime>


ImpulseNoise::ImpulseNoise()
{
}
ImpulseNoise::~ImpulseNoise()
{
}

QString ImpulseNoise::context_menu_str()
{
    return "Add Noise";
}


void ImpulseNoise::processImage(const cv::Mat &inputImage, cv::Mat &outputImage, NoiseType noise_type, const float noise_density)
{

    inputImage.copyTo(outputImage);
    addImpulseNoise<float>(outputImage, noise_density, noise_type);
}



#endif // IMPULSE_NOISE2_IMPL_H
