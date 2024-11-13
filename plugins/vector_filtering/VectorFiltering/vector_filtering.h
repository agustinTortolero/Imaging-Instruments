#ifndef VECTOR_FILTERING_H
#define VECTOR_FILTERING_H
#include <vector>
#include <cstddef> // for size_t

template<typename T>
class ImageFiltering {
public:
    ImageFiltering() = default;
    ~ImageFiltering() = default;

    void median_filter(T* out, const T* in, std::size_t Y, std::size_t X, unsigned char n_threads);
    void vmf(T* out, const T* in, std::size_t Y, std::size_t X, unsigned char n_threads);
    void vmf_2D(T* out, const T* in1, const T* in2, std::size_t Y, std::size_t X, unsigned int n_threads);
    void alpha_vmf_2D(T* out, const T* in1, const T* in2, std::size_t Y, std::size_t X, unsigned int n_threads);

private:
    void getWindow(std::vector<T>& pixels, const T* img, int row, int col, std::size_t X);
    void getWindow(std::vector<T>& R, std::vector<T>& G, std::vector<T>& B, const T* img, int row, int col, std::size_t X);
    void getWindow(std::vector<T>& R, std::vector<T>& G, std::vector<T>& B, const T* img1, const T* img2, int row, int col, std::size_t X);

    T getL1(T r1, T r2, T g1, T g2, T b1, T b2);
    T getL2(T r1, T r2, T g1, T g2, T b1, T b2);

    std::vector<T> getAlphaVmf(const std::vector<T>& vectR, const std::vector<T>& vectG,
                               const std::vector<T>& vectB, std::size_t pixel_count);

    void selectionSort(std::vector<int>& positions, const std::vector<T>& alphaValues);
    void selectionSort(std::vector<T>& pixels);
};

// Explicit template instantiations, con esto genera nomas las que se requieren.
template class ImageFiltering<unsigned char>;// no filtra, necesita especializarse
template class ImageFiltering<float>;
template class ImageFiltering<double>;
#endif // VECTOR_FILTERING_H
