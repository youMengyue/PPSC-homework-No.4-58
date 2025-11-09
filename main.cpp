// main.cpp

/**
 * @file main.cpp
 * @brief Enhanced demonstration of SIMD instruction _mm_div_pd performance advantage
 * @author Student #58
 * 
 * --- Assignment Description ---
 * Student number: 58
 * Instruction derivation:
 * 1. Prefix: 58 % 2 = 0  => _mm
 * 2. Suffix: 58 % 4 = 2  => _pd
 * 3. Core: 58 / 8 = 7    => _div
 * Target instruction: _mm_div_pd
 * 
 * --- Improvements over basic version ---
 * PROBLEM: With simple single-pass array division, the performance difference 
 * between scalar and SIMD versions is small. This occurs because:
 * 1. Division operation itself is very slow (13-16 CPU cycles)
 * 2. For large arrays, most time is spent on memory read/write, not computation
 * 
 * SOLUTION: Increase computational intensity through multiple iterations.
 * Instead of one pass through the array, we perform several passes with multiple 
 * division operations. This allows us to demonstrate the real advantage of SIMD 
 * instructions, as computation time begins to dominate over memory access time.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h> // Header file for SIMD instructions
#include <random>
#include <iomanip>

// ============================================================================
// Scalar version with increased computational intensity
// ============================================================================
void scalar_division_intensive(const std::vector<double>& a, 
                                const std::vector<double>& b, 
                                std::vector<double>& result,
                                int iterations) {
    // Perform multiple iterations of computation to increase workload
    for (int iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < a.size(); ++i) {
            // Three sequential division operations
            result[i] = a[i] / b[i];                    // First division
            result[i] = result[i] / (b[i] + 1.0);       // Second division
            result[i] = result[i] / (a[i] + 2.0);       // Third division
        }
    }
}

// ============================================================================
// SIMD version with increased computational intensity
// ============================================================================
void simd_division_intensive(const std::vector<double>& a, 
                              const std::vector<double>& b, 
                              std::vector<double>& result,
                              int iterations) {
    // Prepare constants for SIMD operations
    __m128d one = _mm_set1_pd(1.0);  // Vector of two 1.0 values
    __m128d two = _mm_set1_pd(2.0);  // Vector of two 2.0 values
    
    // Perform multiple iterations of computation
    for (int iter = 0; iter < iterations; ++iter) {
        // Process 2 doubles at a time (128 bits)
        for (size_t i = 0; i < a.size(); i += 2) {
            // Load data into SIMD registers
            __m128d a_vec = _mm_loadu_pd(&a[i]);
            __m128d b_vec = _mm_loadu_pd(&b[i]);
            __m128d result_vec = _mm_loadu_pd(&result[i]);
            
            // Perform three sequential division operations in parallel on two elements
            result_vec = _mm_div_pd(a_vec, b_vec);                      // First division
            result_vec = _mm_div_pd(result_vec, _mm_add_pd(b_vec, one)); // Second division
            result_vec = _mm_div_pd(result_vec, _mm_add_pd(a_vec, two)); // Third division
            
            // Store result back to memory
            _mm_storeu_pd(&result[i], result_vec);
        }
    }
}

// ============================================================================
// Function for multiple time measurements (for accuracy)
// ============================================================================
template<typename Func>
double benchmark(Func func, int rounds) {
    double total_time = 0.0;
    
    for (int r = 0; r < rounds; ++r) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> elapsed = end - start;
        total_time += elapsed.count();
    }
    
    return total_time / rounds; // Return average time
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ENHANCED SIMD PERFORMANCE DEMONSTRATION          ║" << std::endl;
    std::cout << "║  Student #58: Instruction _mm_div_pd              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // Test configuration parameters
    // ========================================================================
    const size_t ARRAY_SIZE = 10000000;      // 10 million elements
    const int ITERATIONS = 10;                // Number of iterations for intensive computation
    const int BENCHMARK_ROUNDS = 3;           // Number of test repetitions for averaging

    std::cout << "Test parameters:" << std::endl;
    std::cout << "  Array size:         " << ARRAY_SIZE << " elements" << std::endl;
    std::cout << "  Computation iters:  " << ITERATIONS << std::endl;
    std::cout << "  Measurement rounds: " << BENCHMARK_ROUNDS << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // Data initialization
    // ========================================================================
    std::vector<double> dividends(ARRAY_SIZE);
    std::vector<double> divisors(ARRAY_SIZE);
    std::vector<double> result_scalar(ARRAY_SIZE);
    std::vector<double> result_simd(ARRAY_SIZE);

    std::random_device rd;
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<> dis(1.0, 100.0);

    std::cout << "Initializing arrays..." << std::endl;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        dividends[i] = dis(gen) * 5.0;  // Dividend: 5 to 500
        divisors[i] = dis(gen);         // Divisor: 1 to 100
    }
    std::cout << "Initialization complete!" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // PERFORMANCE TESTING
    // ========================================================================
    std::cout << "┌───────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│  INTENSIVE COMPUTATION (multiple divisions)       │" << std::endl;
    std::cout << "└───────────────────────────────────────────────────┘" << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    // Test scalar version
    std::cout << "Running scalar version..." << std::endl;
    double scalar_time = benchmark([&]() {
        scalar_division_intensive(dividends, divisors, result_scalar, ITERATIONS);
    }, BENCHMARK_ROUNDS);

    std::cout << "  ✓ Scalar version completed" << std::endl;
    std::cout << "  Average time: " << scalar_time << " ms" << std::endl;
    std::cout << std::endl;

    // Test SIMD version
    std::cout << "Running SIMD version..." << std::endl;
    double simd_time = benchmark([&]() {
        simd_division_intensive(dividends, divisors, result_simd, ITERATIONS);
    }, BENCHMARK_ROUNDS);

    std::cout << "  ✓ SIMD version completed" << std::endl;
    std::cout << "  Average time: " << simd_time << " ms" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // RESULTS
    // ========================================================================
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  MEASUREMENT RESULTS                              ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  Scalar version (loop):    " << std::setw(10) << scalar_time << " ms      ║" << std::endl;
    std::cout << "║  SIMD version (_mm_div_pd):" << std::setw(10) << simd_time << " ms      ║" << std::endl;
    std::cout << "║  ───────────────────────────────────────────────  ║" << std::endl;

    double speedup = scalar_time / simd_time;
    std::cout << "║  Speedup:                  " << std::setw(10) << speedup << " x       ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // CORRECTNESS VERIFICATION
    // ========================================================================
    std::cout << "Verifying result correctness..." << std::endl;
    
    // Run both algorithms once more for verification
    scalar_division_intensive(dividends, divisors, result_scalar, 1);
    simd_division_intensive(dividends, divisors, result_simd, 1);
    
    bool correct = true;
    double max_error = 0.0;
    
    for (size_t i = 0; i < std::min(size_t(1000), ARRAY_SIZE); ++i) {
        double error = std::abs(result_scalar[i] - result_simd[i]);
        max_error = std::max(max_error, error);
        
        if (error > 1e-9) {
            std::cerr << "✗ ERROR: Result mismatch at index " << i << std::endl;
            std::cerr << "  Scalar value: " << result_scalar[i] << std::endl;
            std::cerr << "  SIMD value:   " << result_simd[i] << std::endl;
            std::cerr << "  Difference:   " << error << std::endl;
            correct = false;
            break;
        }
    }
    
    if (correct) {
        std::cout << "✓ Verification passed successfully!" << std::endl;
        std::cout << "  Maximum error: " << std::scientific << max_error << std::fixed << std::endl;
    }
    std::cout << std::endl;

    // ========================================================================
    // CONCLUSION
    // ========================================================================
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  CONCLUSION                                       ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  The _mm_div_pd instruction processes 2 double    ║" << std::endl;
    std::cout << "║  values simultaneously, providing significant     ║" << std::endl;
    std::cout << "║  speedup for computationally intensive           ║" << std::endl;
    std::cout << "║  operations.                                      ║" << std::endl;
    std::cout << "║                                                   ║" << std::endl;
    std::cout << "║  Increasing computational workload allowed us     ║" << std::endl;
    std::cout << "║  to demonstrate the real advantage of SIMD        ║" << std::endl;
    std::cout << "║  over scalar computations.                        ║" << std::endl;
    std::cout << "║                                                   ║" << std::endl;
    std::cout << "║  To compile use:                                  ║" << std::endl;
    std::cout << "║  make run_no_vec                                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;

    return correct ? 0 : 1;
}
