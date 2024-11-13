#include "color_enhancement.h"


void ColorEnhancement::calculateHistograms(const cv::Mat& img, std::vector<cv::Mat>& hist) {
    for (int i = 0; i < 3; ++i) {
        hist[i] = cv::Mat::zeros(256, 1, CV_32F);
    }

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
            for (int c = 0; c < 3; ++c) {
                hist[c].at<float>(pixel[c])++;
            }
        }
    }
}

cv::Mat ColorEnhancement::histogramEqualization(const cv::Mat& img) {
    cv::Mat hsvImage;
    cv::cvtColor(img, hsvImage, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> channels;
    cv::split(hsvImage, channels);
    cv::Mat i = channels[2]; // Intensity

    // Histogram Equalization
    cv::Mat equalizedI;
    cv::equalizeHist(i, equalizedI);

    // Merge back
    std::vector<cv::Mat> equalizedChannels = {channels[0], channels[1], equalizedI};
    cv::Mat equalizedHsiImage;
    cv::merge(equalizedChannels, equalizedHsiImage);

    cv::Mat equalizedImage;
    cv::cvtColor(equalizedHsiImage, equalizedImage, cv::COLOR_HSV2BGR);

    std::vector<cv::Mat> originalHist(3), equalizedHist(3);
    calculateHistograms(img, originalHist);
    calculateHistograms(equalizedImage, equalizedHist);

    return equalizedImage;
}

cv::Mat ColorEnhancement::adaptiveHistogramEqualization(const cv::Mat& img) {
    cv::Mat hsiImage;
    cv::cvtColor(img, hsiImage, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> channels;
    cv::split(hsiImage, channels);
    cv::Mat i = channels[2]; // Intensity

    // Create CLAHE object
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));

    // Apply CLAHE to the intensity channel
    cv::Mat equalizedI;
    clahe->apply(i, equalizedI);

    // Merge back
    std::vector<cv::Mat> equalizedChannels = {channels[0], channels[1], equalizedI};
    cv::Mat equalizedHsiImage;
    cv::merge(equalizedChannels, equalizedHsiImage);

    cv::Mat equalizedImage;
    cv::cvtColor(equalizedHsiImage, equalizedImage, cv::COLOR_HSV2BGR);

    std::vector<cv::Mat> originalHist(3), equalizedHist(3);
    calculateHistograms(img, originalHist);
    calculateHistograms(equalizedImage, equalizedHist);

    return equalizedImage;
}

cv::Mat ColorEnhancement::claheEqualization(const cv::Mat& img) {
    cv::Mat hsiImage;
    cv::cvtColor(img, hsiImage, cv::COLOR_BGR2HSV); // Use HSV since OpenCV doesn't have HSI

    std::vector<cv::Mat> channels;
    cv::split(hsiImage, channels);
    cv::Mat i = channels[2]; // Intensity channel (V in HSV)

    // Create CLAHE object
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));

    // Apply CLAHE to the intensity channel
    cv::Mat equalizedI;
    clahe->apply(i, equalizedI);

    // Merge back the channels
    std::vector<cv::Mat> equalizedChannels = {channels[0], channels[1], equalizedI};
    cv::Mat equalizedHsiImage;
    cv::merge(equalizedChannels, equalizedHsiImage);

    // Convert back to BGR
    cv::Mat equalizedImage;
    cv::cvtColor(equalizedHsiImage, equalizedImage, cv::COLOR_HSV2BGR); // Convert from HSV back to BGR

    // Calculate histograms
    std::vector<cv::Mat> originalHist(3), equalizedHist(3);
    calculateHistograms(img, originalHist);
    calculateHistograms(equalizedImage, equalizedHist);

    return equalizedImage;
}


void ColorEnhancement::processImage(const cv::Mat& image,cv::Mat &outputImage, const std::string& method) {
    if (image.empty()) {
        std::cerr << "Input image is empty." << std::endl;
        return;
    }
    if (outputImage.empty()) {
        std::cerr << "outputImage image is empty." << std::endl;
        return;
    }

    // Perform histogram equalization based on the specified method
    cv::Mat equalizedImage;
    std::vector<cv::Mat> originalHist(3), equalizedHist(3);

    if (method == "histEq") {
        equalizedImage = histogramEqualization(image);
    } else if (method == "histAdaptive") {
        equalizedImage= adaptiveHistogramEqualization(image);
    } else if (method == "clahe") {
        equalizedImage = claheEqualization(image);
    } else {
        std::cerr << "Unknown method: " << method << std::endl;
        return;
    }

    outputImage = equalizedImage;

}

