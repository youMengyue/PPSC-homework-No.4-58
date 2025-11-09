// main.cpp

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>
#include <random>
#include <iomanip> // 用于格式化输出

// 使用标量计算（传统 for 循环）进行除法
void scalar_division(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& result) {
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] / b[i];
    }
}

// 使用 _mm_div_pd SIMD 指令进行除法
void simd_division(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& result) {
    // 每次处理 2 个 double (2 * 64bit = 128bit)
    for (size_t i = 0; i < a.size(); i += 2) {
        __m128d a_vec = _mm_loadu_pd(&a[i]);
        __m128d b_vec = _mm_loadu_pd(&b[i]);
        __m128d result_vec = _mm_div_pd(a_vec, b_vec);
        _mm_storeu_pd(&result[i], result_vec);
    }
}

int main() {
    std::cout << "--- SIMD Performance Test ---" << std::endl;
    std::cout << "Student ID: 58" << std::endl;
    std::cout << "Target Instruction: _mm_div_pd" << std::endl;
    std::cout << "-----------------------------" << std::endl << std::endl;

    const size_t ARRAY_SIZE = 20000000;
    
    std::vector<double> dividends(ARRAY_SIZE);
    std::vector<double> divisors(ARRAY_SIZE);
    std::vector<double> result_scalar(ARRAY_SIZE);
    std::vector<double> result_simd(ARRAY_SIZE);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 100.0);

    std::cout << "Initializing " << ARRAY_SIZE << " elements..." << std::endl;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        dividends[i] = dis(gen) * 5.0;
        divisors[i] = dis(gen);
    }
    std::cout << "Initialization complete." << std::endl << std::endl;

    // --- 性能测试 ---

    // 1. 测试标量版本
    auto start_scalar = std::chrono::high_resolution_clock::now();
    scalar_division(dividends, divisors, result_scalar);
    auto end_scalar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> scalar_time = end_scalar - start_scalar;
    std::cout << std::fixed << std::setprecision(3); // 设置输出精度
    std::cout << "Scalar (normal loop) division took: " << scalar_time.count() << " ms" << std::endl;

    // 2. 测试 SIMD 版本
    auto start_simd = std::chrono::high_resolution_clock::now();
    simd_division(dividends, divisors, result_simd);
    auto end_simd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> simd_time = end_simd - start_simd;
    std::cout << "SIMD (_mm_div_pd) division took:    " << simd_time.count() << " ms" << std::endl;

    // --- 结论 ---
    std::cout << std::endl;
    if (simd_time.count() > 0 && simd_time < scalar_time) {
        double speedup = scalar_time.count() / simd_time.count();
        std::cout << "Conclusion: SIMD is " << std::setprecision(2) << speedup << " times faster!" << std::endl;
    } else {
        std::cout << "Conclusion: SIMD is not faster. Check compiler flags (ensure -O3 is used)!" << std::endl;
    }

    // (可选) 验证结果是否一致
    for(size_t i = 0; i < 10; ++i) {
        if (std::abs(result_scalar[i] - result_simd[i]) > 1e-9) {
            std::cerr << "Verification FAILED at index " << i << std::endl;
            return 1;
        }
    }
    std::cout << "Verification PASSED for the first 10 elements." << std::endl;

    return 0;
}