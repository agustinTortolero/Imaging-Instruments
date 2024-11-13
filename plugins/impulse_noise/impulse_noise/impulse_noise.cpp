#include "impulse_noise.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <cstdlib>  // For std::rand and std::srand
#include <ctime>    // For std::time

Impulse_noise::Impulse_noise()
{
    // Initialize random seed
    std::srand(std::time(0));
}

Impulse_noise::~Impulse_noise()
{
}

QString Impulse_noise::description()
{
    return "This plugin adds salt and pepper noise to any image."
           " This plugin's goal is to make us more familiar with the"
           " concept of plugins in general.";
}

void Impulse_noise::processImage(const cv::Mat &inputImage, cv::Mat &outputImage)
{
    // Clone input image to output image
    inputImage.copyTo(outputImage);

    // Add salt and pepper noise
    addSaltAndPepperNoise(outputImage, 0.5); // 50% noise ratio
}

void Impulse_noise::addSaltAndPepperNoise(cv::Mat &image, float saltPepperRatio)
{
    // Check for valid ratio
    if (saltPepperRatio < 0 || saltPepperRatio > 1) {
        throw std::invalid_argument("saltPepperRatio should be between 0 and 1.");
    }

    // Number of pixels to alter
    int totalPixels = image.rows * image.cols;
    int numSaltPepperPixels = static_cast<int>(totalPixels * saltPepperRatio);

    // Loop through and apply noise
    for (int i = 0; i < numSaltPepperPixels; ++i) {
        int x = std::rand() % image.cols;
        int y = std::rand() % image.rows;

        // Decide between salt and pepper
        if (std::rand() % 2 == 0) {
            // Salt (white)
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255); // For color image
        } else {
            // Pepper (black)
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0); // For color image
        }
    }
}
