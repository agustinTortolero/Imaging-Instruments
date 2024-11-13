#include "vector_filtering_plugin.h"
#include <omp.h> // Include OpenMP header

VectorFiltering::VectorFiltering()
{
}


VectorFiltering::~VectorFiltering()
{
}


QString VectorFiltering::context_menu_str()
{
    return "Filter";
}

void VectorFiltering::processImage(const cv::Mat &inputImage1, cv::Mat &outputImage)
{
    // Create a temporary image to hold the converted image
    cv::Mat tempImage;

    // Get the maximum number of threads available
    unsigned char nthreads = static_cast<unsigned char>(omp_get_max_threads())-4;

    // Check if the input image is grayscale (1 channel) or color (3 channels)
    if (inputImage1.channels() == 1) {
        // Grayscale image - convert to CV_32FC1 for filtering
        inputImage1.convertTo(tempImage, CV_32FC1);
        outputImage = cv::Mat(inputImage1.size(), CV_32FC1); // Output will also be CV_32FC1

        ImageFiltering<float> filter; // Use float filter
        filter.median_filter(reinterpret_cast<float*>(outputImage.data), reinterpret_cast<float*>(tempImage.data), inputImage1.rows, inputImage1.cols, nthreads);

        // Convert back to 8-bit
        outputImage.convertTo(outputImage, CV_8UC1);
    } else if (inputImage1.channels() == 3) {
        // Color image - convert to CV_32FC3 for filtering
        inputImage1.convertTo(tempImage, CV_32FC3);
        outputImage = cv::Mat(inputImage1.size(), CV_32FC3); // Output will also be CV_32FC3

        ImageFiltering<float> filter; // Use float filter
        filter.vmf(reinterpret_cast<float*>(outputImage.data), reinterpret_cast<float*>(tempImage.data), inputImage1.rows, inputImage1.cols, nthreads);

        // Convert back to 8-bit
        outputImage.convertTo(outputImage, CV_8UC3);
    } else {
        outputImage = inputImage1.clone(); // Optionally, copy input to output if unsupported
    }
}

/*
void VectorFiltering::processImage(const cv::Mat &inputImage1, const cv::Mat &inputImage2, cv::Mat &outputImage)
{
    // Create temporary images for conversions
    cv::Mat tempImage1, tempImage2;

    // Get the maximum number of threads available
    unsigned char nthreads = static_cast<unsigned char>(omp_get_max_threads());

    // Check if both input images are color images
    if (inputImage1.channels() == 3 && inputImage2.channels() == 3) {
        // Convert to CV_32FC3 for processing
        inputImage1.convertTo(tempImage1, CV_32FC3);
        inputImage2.convertTo(tempImage2, CV_32FC3);
        outputImage = cv::Mat(inputImage1.size(), CV_32FC3); // Output will also be CV_32FC3

        ImageFiltering<float> filter; // Use float filter
        filter.vmf_2D(reinterpret_cast<float*>(outputImage.data), reinterpret_cast<float*>(tempImage1.data), reinterpret_cast<float*>(tempImage2.data), inputImage1.rows, inputImage1.cols, nthreads);

        // Convert back to 8-bit
        outputImage.convertTo(outputImage, CV_8UC3);
    } else {
        outputImage = inputImage1.clone(); // Optionally, copy the first input to output if unsupported
    }
}
*/
