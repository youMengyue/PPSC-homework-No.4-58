// main.cpp

/**
 * @file main.cpp
 * @brief Демонстрация преимущества в производительности инструкции SIMD _mm_div_pd (деление)
 * @author Студент № 58
 * 
 * --- Описание задания ---
 * Номер студента: 58
 * Итоговая инструкция: _mm_div_pd
 * 
 * --- Цель программы ---
 * Цель этой программы - наглядно продемонстрировать повышение производительности при использовании 
 * инструкции _mm_div_pd по сравнению с "честным" скалярным кодом.
 * 
 * Современные компиляторы с флагом оптимизации -O3 автоматически векторизуют простые циклы, 
 * из-за чего сравнение ручного SIMD-кода с обычным циклом for не показывает разницы.
 * Чтобы увидеть реальное преимущество, мы используем специальный флаг компилятора 
 * `-fno-tree-vectorize` (в Makefile, цель `run_no_vec`), который запрещает эту автоматическую 
 * оптимизацию. Это заставляет компилятор генерировать действительно последовательный, 
 * поэлементный код для скалярной версии, что позволяет корректно сравнить его с 
 * нашим SIMD-вариантом.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h> // Заголовочный файл для инструкций SIMD
#include <random>
#include <iomanip>     // Для форматирования вывода

// Скалярное деление (стандартный цикл for)
// При компиляции с -fno-tree-vectorize этот цикл НЕ будет автоматически векторизован
void scalar_division(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& result) {
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] / b[i];
    }
}

// Деление с использованием SIMD-инструкции _mm_div_pd
void simd_division(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& result) {
    // Обрабатываем по 2 double за раз (2 * 64 бит = 128 бит)
    for (size_t i = 0; i < a.size(); i += 2) {
        __m128d a_vec = _mm_loadu_pd(&a[i]);
        __m128d b_vec = _mm_loadu_pd(&b[i]);
        __m128d result_vec = _mm_div_pd(a_vec, b_vec);
        _mm_storeu_pd(&result[i], result_vec);
    }
}

int main() {
    std::cout << "--- Тест производительности SIMD ---" << std::endl;
    std::cout << "Номер студента: 58" << std::endl;
    std::cout << "Целевая инструкция: _mm_div_pd" << std::endl;
    std::cout << "------------------------------------" << std::endl << std::endl;

    const size_t ARRAY_SIZE = 20000000;
    
    std::vector<double> dividends(ARRAY_SIZE);
    std::vector<double> divisors(ARRAY_SIZE);
    std::vector<double> result_scalar(ARRAY_SIZE);
    std::vector<double> result_simd(ARRAY_SIZE);

    // --- Инициализация данных ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 100.0);

    std::cout << "Инициализация " << ARRAY_SIZE << " элементов..." << std::endl;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        dividends[i] = dis(gen) * 5.0;
        divisors[i] = dis(gen);
    }
    std::cout << "Инициализация завершена." << std::endl << std::endl;

    // --- Тестирование производительности ---

    // 1. Тестируем скалярную версию
    auto start_scalar = std::chrono::high_resolution_clock::now();
    scalar_division(dividends, divisors, result_scalar);
    auto end_scalar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> scalar_time = end_scalar - start_scalar;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Скалярное деление (обычный цикл) заняло: " << scalar_time.count() << " мс" << std::endl;

    // 2. Тестируем SIMD-версию
    auto start_simd = std::chrono::high_resolution_clock::now();
    simd_division(dividends, divisors, result_simd);
    auto end_simd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> simd_time = end_simd - start_simd;
    std::cout << "SIMD деление (_mm_div_pd) заняло:       " << simd_time.count() << " мс" << std::endl;

    // --- Вывод ---
    std::cout << std::endl;
    if (simd_time.count() > 0 && simd_time < scalar_time) {
        double speedup = scalar_time.count() / simd_time.count();
        std::cout << "Заключение: SIMD в " << std::setprecision(2) << speedup << " раз быстрее!" << std::endl;
    } else {
        std::cout << "Заключение: SIMD не быстрее. Проверьте флаги компилятора, возможно, автовекторизация не была отключена." << std::endl;
    }

    // (Опционально) Проверяем, совпадают ли результаты
    for(size_t i = 0; i < 10; ++i) {
        if (std::abs(result_scalar[i] - result_simd[i]) > 1e-9) {
            std::cerr << "ПРОВЕРКА НЕ ПРОШЛА по индексу " << i << std::endl;
            return 1;
        }
    }
    std::cout << "Проверка первых 10 элементов ПРОШЛА успешно." << std::endl;

    return 0;
}
