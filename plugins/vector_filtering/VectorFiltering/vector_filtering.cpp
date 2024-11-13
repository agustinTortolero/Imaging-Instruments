#include "vector_filtering.h"
#include <cmath>
#include <omp.h>
#include <algorithm>
#include <numeric>

// Template function implementations for ImageFiltering

template<typename T>
void ImageFiltering<T>::getWindow(std::vector<T>& pixels, const T* img, int row, int col, std::size_t X) {
    int index = 0;
    int k = 0; // To keep track of the index in the pixels vector
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {

                index = (row + i) * X + (col + j);
                pixels[k++] = img[index];

        }
    }
}

template<typename T>
void ImageFiltering<T>::getWindow(std::vector<T>& R, std::vector<T>& G, std::vector<T>& B, const T* img, int row, int col, std::size_t X) {
    int index = 0;
    int k = 0; // To keep track of the index in the R, G, B vectors
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // Ensure that the indices are within bounds

                index = (row + i) * X + (col + j);
                R[k] = img[index * 3];     // Red channel
                G[k] = img[index * 3 + 1]; // Green channel
                B[k] = img[index * 3 + 2]; // Blue channel
                k++;

        }
    }
}

template<typename T>
void ImageFiltering<T>::getWindow(std::vector<T>& R, std::vector<T>& G, std::vector<T>& B, const T* img1, const T* img2, int row, int col, std::size_t X) {
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

template<typename T>
T ImageFiltering<T>::getL1(T r1, T r2, T g1, T g2, T b1, T b2) {
    // Convert to float or int for safe arithmetic operations
    // We use static_cast to ensure proper conversion
    return std::fabs(static_cast<float>(r1) - static_cast<float>(r2)) +
           std::fabs(static_cast<float>(g1) - static_cast<float>(g2)) +
           std::fabs(static_cast<float>(b1) - static_cast<float>(b2));
}
template<typename T>
T ImageFiltering<T>::getL2(T r1, T r2, T g1, T g2, T b1, T b2) {
    // Convert to float or int for safe arithmetic operations
    // We use static_cast to ensure proper conversion
    T dr = static_cast<float>(r1) - static_cast<float>(r2);
    T dg = static_cast<float>(g1) - static_cast<float>(g2);
    T db = static_cast<float>(b1) - static_cast<float>(b2);

    // Compute the L2 distance (Euclidean distance)
    return std::sqrt(dr * dr + dg * dg + db * db);
}



template<typename T>
std::vector<T> ImageFiltering<T>::getAlphaVmf(const std::vector<T>& vectR, const std::vector<T>& vectG,
                                              const std::vector<T>& vectB, std::size_t pixel_count) {
    std::vector<T> alpha_values(pixel_count, 0.0);

    for (std::size_t F = 0; F < pixel_count; ++F) {
        for (std::size_t x = F + 1; x < pixel_count; ++x) {
            T disteucl = getL1(vectR[F], vectR[x], vectG[F], vectG[x], vectB[F], vectB[x]);
            alpha_values[F] += disteucl;
            alpha_values[x] += disteucl;
        }
    }
    return alpha_values;
}

template<typename T>
void ImageFiltering<T>::selectionSort(std::vector<int>& positions, const std::vector<T>& alphaValues) {
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

template<typename T>
void ImageFiltering<T>::selectionSort(std::vector<T>& pixels) {
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

template<typename T>
void ImageFiltering<T>::median_filter(T* out, const T* in, std::size_t Y, std::size_t X, unsigned char n_threads) {


#pragma omp parallel num_threads(n_threads)
{
    #pragma omp for schedule(static)
    for (int row = 1; row < Y - 1; row++) {
        for (int col = 1; col < X - 1; col++) {
            std::vector<T> pixels(9, 0.0);
            getWindow(pixels, in, row, col, X);
            selectionSort(pixels);
            out[(row * X + col)] = pixels[4]; // Index 4 for median in a 3x3 window
        }
    }
}
}

template<typename T>
void ImageFiltering<T>::vmf(T* out, const T* in, std::size_t Y, std::size_t X, unsigned char n_threads) {
    #pragma omp parallel num_threads(n_threads)
    {
        #pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            for (int col = 1; col < X - 1; col++) {
                std::vector<T> vectR(9, 0.0), vectG(9, 0.0), vectB(9, 0.0);
                getWindow(vectR, vectG, vectB, in, row, col, X);

                std::vector<T> alphaValues = getAlphaVmf(vectR, vectG, vectB, 9);

                std::vector<int> positions(9);
                std::iota(positions.begin(), positions.end(), 0);

                selectionSort(positions, alphaValues);

                out[(row * X + col) * 3 + 0] = vectR[positions[0]];
                out[(row * X + col) * 3 + 1] = vectG[positions[0]];
                out[(row * X + col) * 3 + 2] = vectB[positions[0]];
            }
        }
    }
}

template<typename T>
void ImageFiltering<T>::vmf_2D(T* out, const T* in1, const T* in2, std::size_t Y, std::size_t X, unsigned int n_threads) {
    #pragma omp parallel num_threads(n_threads)
    {
        #pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            std::vector<T> R(18, 0.0), G(18, 0.0), B(18, 0.0), alphas(18, 0.0);
            std::vector<int> indices(18);

            for (int col = 1; col < X - 1; col++) {
                getWindow(R, G, B, in1, in2, row, col, X);
                alphas = getAlphaVmf(R, G, B, 18);

                // Initialize indices
                std::iota(indices.begin(), indices.end(), 0);

                // Sort indices based on alphaValues
                selectionSort(indices, alphas);

                // Choose the position with the smallest alpha value
                int min_pos = indices[0];

                // Assign values to output
                out[(row * X + col) * 3 + 0] = R[min_pos];
                out[(row * X + col) * 3 + 1] = G[min_pos];
                out[(row * X + col) * 3 + 2] = B[min_pos];
            }
        }
    }
}


template<typename T>
void ImageFiltering<T>::alpha_vmf_2D(T* out, const T* in1, const T* in2, std::size_t Y, std::size_t X, unsigned int n_threads) {
    #pragma omp parallel num_threads(n_threads)
    {
        #pragma omp for schedule(static)
        for (int row = 1; row < Y - 1; row++) {
            std::vector<T> R(18, 0.0), G(18, 0.0), B(18, 0.0), alphas(18, 0.0);
            std::vector<int> indices(18);

            for (int col = 1; col < X - 1; col++) {
                getWindow(R, G, B, in1, in2, row, col, X);
                alphas = getAlphaVmf(R, G, B, 18);

                // Initialize indices
                std::iota(indices.begin(), indices.end(), 0);

                // Sort indices based on alphaValues
                selectionSort(indices, alphas);


                // Assign values to output
                out[(row * X + col) * 3 + 0] = (R[indices[0]] + R[indices[1]] + R[indices[2]]) / 3;
                out[(row * X + col) * 3 + 1] = (G[indices[0]] + G[indices[1]] + G[indices[2]]) / 3;
                out[(row * X + col) * 3 + 2] = (B[indices[0]] + B[indices[1]] + B[indices[2]]) / 3;
            }
        }
    }
}

