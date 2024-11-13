#include "vector_filtering_lib.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // Create an instance of ImageFiltering
    ImageFiltering filter; // No template parameter needed

    // Read the image from the specified path
    cv::Mat inputImage = cv::imread("C:/Users/agust/OneDrive/Documentos/RealMadrid_SP05.png", cv::IMREAD_COLOR);

    // Check if the image was loaded successfully
    if (inputImage.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Create a Mat to hold the output image
    cv::Mat outputImage;
    outputImage.create(inputImage.size(), CV_32FC3); // Ensure output image is initialized to float type

    // Run the filter
    filter.run_filter(inputImage, outputImage);

    // Convert output image back to unsigned char for display
    cv::Mat outputImageDisplay;
    outputImage.convertTo(outputImageDisplay, CV_8UC3, 1.0); // Convert to 8-bit for display

    // Display the original and the filtered image
    cv::imshow("Original Image", inputImage);
    cv::imshow("Filtered Image", outputImageDisplay);

    // Wait for a key press indefinitely
    cv::waitKey(0);

    return 0;
}
