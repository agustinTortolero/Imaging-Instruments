#include "vector_filtering_lib.h"

void ImageFiltering::getWindow(std::vector<float>& pixels, const float* img, int row, int col, std::size_t X) {
    int index = 0;
    int k = 0; // To keep track of the index in the pixels vector
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            index = (row + i) * X + (col + j);
            pixels[k++] = img[index];
        }
    }
}

void ImageFiltering::getWindow(std::vector<float>& R, std::vector<float>& G, std::vector<float>& B, const float* img, int row, int col, std::size_t X) {
    int index = 0;
    int k = 0; // To keep track of the index in the R, G, B vectors
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            index = (row + i) * X + (col + j);
            R[k] = img[index * 3];     // Red channel
            G[k] = img[index * 3 + 1]; // Green channel
            B[k] = img[index * 3 + 2]; // Blue channel
            k++;
        }
    }
}

void ImageFiltering::getWindow(std::vector<float>& R, std::vector<float>& G, std::vector<float>& B, const float* img1, const float* img2, int row, int col, std::size_t X) {
    int index = 0;
    int k = 0; // To keep track of the index in the R, G, B vectors

    // Process the first image (img1)
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            index = (row + i) * X + (col + j);
            R[k] = img1[index * 3];     // Red channel
            G[k] = img1[index * 3 + 1]; // Green channel
            B[k] = img1[index * 3 + 2]; // Blue channel
            k++;
        }
    }

    // Process the second image (img2)
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            index = (row + i) * X + (col + j);
            R[k] = img2[index * 3];     // Red channel
            G[k] = img2[index * 3 + 1]; // Green channel
            B[k] = img2[index * 3 + 2]; // Blue channel
            k++;
        }
    }
}

float ImageFiltering::getL1(float r1, float r2, float g1, float g2, float b1, float b2) {
    return std::fabs(r1 - r2) + std::fabs(g1 - g2) + std::fabs(b1 - b2);
}

float ImageFiltering::getL2(float r1, float r2, float g1, float g2, float b1, float b2) {
    float dr = r1 - r2;
    float dg = g1 - g2;
    float db = b1 - b2;
    return std::sqrt(dr * dr + dg * dg + db * db);
}

std::vector<float> ImageFiltering::getAlphaVmf(const std::vector<float>& vectR, const std::vector<float>& vectG,
                                               const std::vector<float>& vectB, std::size_t pixel_count) {
    std::vector<float> alpha_values(pixel_count, 0.0);

    for (std::size_t F = 0; F < pixel_count; ++F) {
        for (std::size_t x = F + 1; x < pixel_count; ++x) {
            float disteucl = getL1(vectR[F], vectR[x], vectG[F], vectG[x], vectB[F], vectB[x]);
            alpha_values[F] += disteucl;
            alpha_values[x] += disteucl;
        }
    }
    return alpha_values;
}

void ImageFiltering::selectionSort(std::vector<int>& positions, const std::vector<float>& alphaValues) {
    for (std::size_t i = 0; i < positions.size() - 1; ++i) {
        std::size_t minIndex = i;
        for (std::size_t j = i + 1; j < positions.size(); ++j) {
            if (alphaValues[positions[j]] < alphaValues[positions[minIndex]]) {
                minIndex = j;
            }
        }
        std::swap(positions[i], positions[minIndex]);
    }
}

void ImageFiltering::selectionSort(std::vector<float>& pixels) {
    for (std::size_t i = 0; i < pixels.size() - 1; ++i) {
        std::size_t minIndex = i;
        for (std::size_t j = i + 1; j < pixels.size(); ++j) {
            if (pixels[j] < pixels[minIndex]) {
                minIndex = j;
            }
        }
        std::swap(pixels[i], pixels[minIndex]);
    }
}

void ImageFiltering::median_filter(float* out, const float* in, std::size_t Y, std::size_t X, unsigned char n_threads) {
#pragma omp parallel num_threads(n_threads)
    {
#pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            for (int col = 1; col < X - 1; col++) {
                std::vector<float> pixels(9, 0.0);
                getWindow(pixels, in, row, col, X);
                selectionSort(pixels);
                out[(row * X + col)] = pixels[4]; // Index 4 for median in a 3x3 window
            }
        }
    }
}

void ImageFiltering::vmf(float* out, const float* in, std::size_t Y, std::size_t X, unsigned char n_threads) {
#pragma omp parallel num_threads(n_threads)
    {
#pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            for (int col = 1; col < X - 1; col++) {
                std::vector<float> vectR(9, 0.0), vectG(9, 0.0), vectB(9, 0.0);
                getWindow(vectR, vectG, vectB, in, row, col, X);
                auto alphaValues = getAlphaVmf(vectR, vectG, vectB, 9);
                std::vector<int> positions = {0, 1, 2, 3, 4, 5, 6, 7, 8};
                selectionSort(positions, alphaValues);
                float r = 0, g = 0, b = 0;
                for (int i = 0; i < 3; ++i) {
                    r += vectR[positions[i]];
                    g += vectG[positions[i]];
                    b += vectB[positions[i]];
                }
                out[(row * X + col) * 3] = r / 3;
                out[(row * X + col) * 3 + 1] = g / 3;
                out[(row * X + col) * 3 + 2] = b / 3;
            }
        }
    }
}

void ImageFiltering::vmf_2D(float* out, const float* in1, const float* in2, std::size_t Y, std::size_t X, unsigned int n_threads) {
#pragma omp parallel num_threads(n_threads)
    {
#pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            for (int col = 1; col < X - 1; col++) {
                std::vector<float> vectR1(9, 0.0), vectG1(9, 0.0), vectB1(9, 0.0);
                std::vector<float> vectR2(9, 0.0), vectG2(9, 0.0), vectB2(9, 0.0);
                getWindow(vectR1, vectG1, vectB1, in1, row, col, X);
                getWindow(vectR2, vectG2, vectB2, in2, row, col, X);

                auto alphaValues1 = getAlphaVmf(vectR1, vectG1, vectB1, 9);
                auto alphaValues2 = getAlphaVmf(vectR2, vectG2, vectB2, 9);
                std::vector<int> positions1 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
                std::vector<int> positions2 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
                selectionSort(positions1, alphaValues1);
                selectionSort(positions2, alphaValues2);

                float r1 = 0, g1 = 0, b1 = 0;
                float r2 = 0, g2 = 0, b2 = 0;
                for (int i = 0; i < 3; ++i) {
                    r1 += vectR1[positions1[i]];
                    g1 += vectG1[positions1[i]];
                    b1 += vectB1[positions1[i]];
                    r2 += vectR2[positions2[i]];
                    g2 += vectG2[positions2[i]];
                    b2 += vectB2[positions2[i]];
                }
                out[(row * X + col) * 3] = (r1 + r2) / 6; // Average for both images
                out[(row * X + col) * 3 + 1] = (g1 + g2) / 6;
                out[(row * X + col) * 3 + 2] = (b1 + b2) / 6;
            }
        }
    }
}

void ImageFiltering::run_filter(const cv::Mat &inputImage1, cv::Mat &outputImage) {
    cv::Mat input_float;
    inputImage1.convertTo(input_float, CV_32F); // Convert input image to float
    outputImage.create(inputImage1.size(), CV_32FC3); // Create output image

    float* in_data = (float*)input_float.data;
    float* out_data = (float*)outputImage.data;

    unsigned char num_threads = omp_get_max_threads();
    vmf(out_data, in_data, inputImage1.rows, inputImage1.cols, num_threads);
}
