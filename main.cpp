// main.cpp

/**
 * @file main.cpp
 * @brief Демонстрация преимущества в производительности инструкции SIMD _mm_div_pd
 * @author Студент № 58
 * 
 * --- Описание задания ---
 * Номер студента: 58
 * Вывод инструкции:
 * 1. Префикс: 58 % 2 = 0  => _mm
 * 2. Суффикс: 58 % 4 = 2  => _pd
 * 3. Ядро: 58 / 8 = 7     => _div
 * Итоговая инструкция: _mm_div_pd
 * 
 * --- Цель программы ---
 * Цель этой программы - наглядно продемонстрировать значительное повышение производительности 
 * при использовании инструкции _mm_div_pd из набора SSE2 для векторизованных вычислений 
 * по сравнению с традиционными скалярными вычислениями (поэлементными в цикле).
 * Инструкция _mm_div_pd способна одновременно выполнять деление над двумя числами типа double, 
 * достигая параллелизма данных и существенно повышая эффективность вычислительно-интенсивных задач.
 * Мы создадим два больших массива, вычислим их частное обоими методами и сравним время выполнения, 
 * чтобы показать разницу.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h> // Заголовочный файл для инструкций SIMD
#include <random>
#include <iomanip> // Для форматирования вывода

// Скалярное деление (стандартный цикл for)
void scalar_division(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& result) {
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] / b[i];
    }
}

// Деление с использованием SIMD-инструкции _mm_div_pd
void simd_division(const std.vector<double>& a, const std.vector<double>& b, std::vector<double>& result) {
    // Обрабатываем по 2 double за раз (2 * 64 бит = 128 бит)
    for (size_t i = 0; i < a.size(); i += 2) {
        // 1. Загружаем 2 значения double из памяти в 128-битный регистр
        // Используется _mm_loadu_pd, так как она не требует выравнивания памяти, что делает код более универсальным
        __m128d a_vec = _mm_loadu_pd(&a[i]);
        __m128d b_vec = _mm_loadu_pd(&b[i]);
        
        // 2. Выполняем основную инструкцию: параллельное деление данных в двух регистрах
        __m128d result_vec = _mm_div_pd(a_vec, b_vec);
        
        // 3. Сохраняем результат из регистра обратно в память
        _mm_storeu_pd(&result[i], result_vec);
    }
}

int main() {
    std::cout << "--- Тест производительности SIMD ---" << std::endl;
    std::cout << "Номер студента: 58" << std::endl;
    std::cout << "Целевая инструкция: _mm_div_pd" << std::endl;
    std::cout << "------------------------------------" << std::endl << std::endl;

    // Определяем достаточно большой размер массива, чтобы разница в производительности была заметна
    const size_t ARRAY_SIZE = 20000000;
    
    std::vector<double> dividends(ARRAY_SIZE);
    std::vector<double> divisors(ARRAY_SIZE);
    std::vector<double> result_scalar(ARRAY_SIZE);
    std::vector<double> result_simd(ARRAY_SIZE);

    // --- Инициализация данных ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 100.0); // Гарантируем, что делитель не равен нулю

    std::cout << "Инициализация " << ARRAY_SIZE << " элементов..." << std::endl;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        dividends[i] = dis(gen) * 5.0; // Делимое
        divisors[i] = dis(gen);       // Делитель
    }
    std::cout << "Инициализация завершена." << std::endl << std::endl;

    // --- Тестирование производительности ---

    // 1. Тестируем скалярную версию
    auto start_scalar = std::chrono::high_resolution_clock::now();
    scalar_division(dividends, divisors, result_scalar);
    auto end_scalar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> scalar_time = end_scalar - start_scalar;
    std::cout << std::fixed << std::setprecision(3); // Устанавливаем точность вывода
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
        std::cout << "Заключение: SIMD не быстрее. Проверьте флаги компилятора (убедитесь, что используется -O3)!" << std::endl;
    }

    // (Опционально) Проверяем, совпадают ли результаты, чтобы избежать ошибок в реализации SIMD
    for(size_t i = 0; i < 10; ++i) { // Проверяем только первые несколько элементов
        if (std::abs(result_scalar[i] - result_simd[i]) > 1e-9) {
            std::cerr << "ПРОВЕРКА НЕ ПРОШЛА по индексу " << i << std::endl;
            return 1;
        }
    }
    std::cout << "Проверка первых 10 элементов ПРОШЛА успешно." << std::endl;

    return 0;
}