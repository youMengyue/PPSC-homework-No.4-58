#include <immintrin.h>
#include <iostream>
#include "../include/simd_utils.hpp"

/*
 * ======================================
 * SIMD Homework — Fan Zhanhong (ID 58)
 * Task: Use `_mm_div_pd` (SSE double division)
 * ======================================
 */

int main() {
    constexpr size_t N = 4; // 向量长度（必须是偶数）
    alignas(16) double a[N] = {8.0, 16.0, 20.0, 100.0};
    alignas(16) double b[N] = {2.0, 4.0, 10.0, 25.0};
    alignas(16) double result[N];

    for (size_t i = 0; i < N; i += 2) {
        __m128d va = _mm_load_pd(&a[i]);   // 加载两个 double
        __m128d vb = _mm_load_pd(&b[i]);   // 加载两个 double
        __m128d vr = _mm_div_pd(va, vb);   // 并行除法操作
        _mm_store_pd(&result[i], vr);      // 存储结果
    }

    std::cout << "Vector A: ";
    print_vector(a, N);
    std::cout << "Vector B: ";
    print_vector(b, N);
    std::cout << "Result  : ";
    print_vector(result, N);

    return 0;
}
