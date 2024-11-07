#ifndef GPU_FILTERING_H
#define GPU_FILTERING_H

#include <iostream>

extern "C" {
    void run_gpu_filter(float* img_filtered, const float* img_noisy, size_t Y, size_t X);
}


#endif // GPU_FILTERING_H
