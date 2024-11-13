#ifndef VECTOR_FILTERING_LIB_H
#define VECTOR_FILTERING_LIB_H

#include <vector>
#include <cstddef> // for size_t
#include <cmath>
#include <omp.h>
#include <algorithm>
#include <numeric>
#include "opencv2/opencv.hpp"


#ifdef BUILDING_DLL
#define DELLEXPORT __declspec(dllexport)
#else
#define DELLEXPORT __declspec(dllimport)
#endif

class ImageFiltering {
public:
    ImageFiltering() = default;
    ~ImageFiltering() = default;

    DELLEXPORT void run_filter(const cv::Mat &inputImage1, cv::Mat &outputImage);

protected:
    void median_filter(float* out, const float* in, std::size_t Y, std::size_t X, unsigned char n_threads);
    void vmf(float* out, const float* in, std::size_t Y, std::size_t X, unsigned char n_threads);
    void vmf_2D(float* out, const float* in1, const float* in2, std::size_t Y, std::size_t X, unsigned int n_threads);

private:
    void alpha_vmf_2D(float* out, const float* in1, const float* in2, std::size_t Y, std::size_t X, unsigned int n_threads);

    void getWindow(std::vector<float>& pixels, const float* img, int row, int col, std::size_t X);
    void getWindow(std::vector<float>& R, std::vector<float>& G, std::vector<float>& B, const float* img, int row, int col, std::size_t X);
    void getWindow(std::vector<float>& R, std::vector<float>& G, std::vector<float>& B, const float* img1, const float* img2, int row, int col, std::size_t X);

    float getL1(float r1, float r2, float g1, float g2, float b1, float b2);
    float getL2(float r1, float r2, float g1, float g2, float b1, float b2);

    std::vector<float> getAlphaVmf(const std::vector<float>& vectR, const std::vector<float>& vectG,
                                   const std::vector<float>& vectB, std::size_t pixel_count);

    void selectionSort(std::vector<int>& positions, const std::vector<float>& alphaValues);
    void selectionSort(std::vector<float>& pixels);
};

#endif // VECTOR_FILTERING_LIB_H
