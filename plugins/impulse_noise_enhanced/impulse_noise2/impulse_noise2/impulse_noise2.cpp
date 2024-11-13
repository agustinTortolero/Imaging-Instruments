#include "impulse_noise2.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <cstdlib>
#include <ctime>
#include <random>
#include <functional>



template <typename T>
void addImpulseNoise(cv::Mat& image, double noiseRatio, NoiseType noiseType);


template <typename T>
void applyNoise(cv::Mat& image, double noiseRatio, NoiseType noiseType,
                std::function<T()> saltAndPepperFunc,
                std::function<T()> randomValueFunc) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);


    //#pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            if (dis(gen) < noiseRatio) {
                if (noiseType == NoiseType::SALT_AND_PEPPER) {
                    image.at<T>(i, j) = saltAndPepperFunc();
                } else if (noiseType == NoiseType::RANDOM_VALUE) {
                    image.at<T>(i, j) = randomValueFunc();
                }
            }
        }
    }
}

// Specialization for uchar (8-bit grayscale images)
template <>
void addImpulseNoise<uchar>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = []() -> uchar { return (rand() % 2) * 255; };
    auto randomValueFunc = []() -> uchar { return static_cast<uchar>(rand() % 256); };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<uchar()>>(saltAndPepperFunc),
               static_cast<std::function<uchar()>>(randomValueFunc));
}

// Specialization for double (normalized grayscale images)
template <>
void addImpulseNoise<double>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = []() -> double { return rand() % 2 ? 0.0 : 1.0; };
    auto randomValueFunc = []() -> double { return static_cast<double>(rand()) / RAND_MAX; };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<double()>>(saltAndPepperFunc),
               static_cast<std::function<double()>>(randomValueFunc));
}

// Specialization for float (normalized grayscale images)
template <>
void addImpulseNoise<float>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = []() -> float { return rand() % 2 ? 0.0f : 1.0f; };
    auto randomValueFunc = []() -> float { return static_cast<float>(rand()) / RAND_MAX; };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<float()>>(saltAndPepperFunc),
               static_cast<std::function<float()>>(randomValueFunc));
}

// Specialization for cv::Vec3b (color images with uchar channels)
template <>
void addImpulseNoise<cv::Vec3b>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = [&]() -> cv::Vec3b {
        cv::Vec3b pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = (rand() % 2) * 255;
        }
        return pixel;
    };

    auto randomValueFunc = [&]() -> cv::Vec3b {
        cv::Vec3b pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = rand() % 256;
        }
        return pixel;
    };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<cv::Vec3b()>>(saltAndPepperFunc),
               static_cast<std::function<cv::Vec3b()>>(randomValueFunc));
}

// Specialization for cv::Vec3d (color images with double channels)
template <>
void addImpulseNoise<cv::Vec3d>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = [&]() -> cv::Vec3d {
        cv::Vec3d pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = rand() % 2 ? 0.0 : 1.0;
        }
        return pixel;
    };

    auto randomValueFunc = [&]() -> cv::Vec3d {
        cv::Vec3d pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = static_cast<double>(rand()) / RAND_MAX;
        }
        return pixel;
    };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<cv::Vec3d()>>(saltAndPepperFunc),
               static_cast<std::function<cv::Vec3d()>>(randomValueFunc));
}

// Specialization for cv::Vec3f (color images with float channels)
template <>
void addImpulseNoise<cv::Vec3f>(cv::Mat& image, double noiseRatio, NoiseType noiseType) {
    auto saltAndPepperFunc = [&]() -> cv::Vec3f {
        cv::Vec3f pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = rand() % 2 ? 0.0f : 1.0f;
        }
        return pixel;
    };

    auto randomValueFunc = [&]() -> cv::Vec3f {
        cv::Vec3f pixel;
        for (int k = 0; k < 3; ++k) {
            pixel[k] = static_cast<float>(rand()) / RAND_MAX;
        }
        return pixel;
    };

    applyNoise(image, noiseRatio, noiseType,
               static_cast<std::function<cv::Vec3f()>>(saltAndPepperFunc),
               static_cast<std::function<cv::Vec3f()>>(randomValueFunc));
}




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


void ImpulseNoise::processImage(const cv::Mat &inputImage, cv::Mat &outputImage, const float noise_density)
{
    inputImage.copyTo(outputImage);

    NoiseType noiseType = NoiseType::SALT_AND_PEPPER;

    // Process the image based on its type
    if (inputImage.type() == CV_8UC1) {
        addImpulseNoise<uchar>(outputImage, noise_density, noiseType);
    } else if (inputImage.type() == CV_32FC1) {
        addImpulseNoise<float>(outputImage, noise_density, noiseType);
    } else if (inputImage.type() == CV_64FC1) {
        addImpulseNoise<double>(outputImage, noise_density, noiseType);
    } else if (inputImage.type() == CV_8UC3) {
        addImpulseNoise<cv::Vec3b>(outputImage, noise_density, noiseType);
    } else if (inputImage.type() == CV_32FC3) {
        addImpulseNoise<cv::Vec3f>(outputImage, noise_density, noiseType);
    } else if (inputImage.type() == CV_64FC3) {
        addImpulseNoise<cv::Vec3d>(outputImage, noise_density, noiseType);
    }
}



void ImpulseNoise::addSaltAndPepperNoise(cv::Mat &image, const float noise_denisity){
    // Check for valid ratio
    if (noise_denisity < 0 || noise_denisity > 1) {
        throw std::invalid_argument("saltPepperRatio should be between 0 and 1.");
    }

    // Number of pixels to alter
    int totalPixels = image.rows * image.cols;
    int numSaltPepperPixels = static_cast<int>(totalPixels * noise_denisity);

    // Loop through and apply noise
    for (int i = 0; i < numSaltPepperPixels; ++i) {
        int x = std::rand() % image.cols;
        int y = std::rand() % image.rows;

        // Decide between salt and pepper
        if (std::rand() % 2 == 0) {
            // Salt (white)
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
        } else {
            // Pepper (black)
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
        }
    }
}


