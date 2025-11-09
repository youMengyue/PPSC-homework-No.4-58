// main.cpp

/**
 * @file main.cpp
 * @brief Улучшенная демонстрация преимущества инструкции SIMD _mm_div_pd
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
 * --- Улучшения по сравнению с базовой версией ---
 * ПРОБЛЕМА: При простом однократном делении массивов разница в производительности 
 * между скалярной и SIMD-версией оказывается небольшой. Это происходит потому, что:
 * 1. Операция деления сама по себе очень медленная (13-16 тактов процессора)
 * 2. При больших массивах основное время уходит на чтение/запись памяти, а не на вычисления
 * 
 * РЕШЕНИЕ: Увеличить вычислительную интенсивность через многократные итерации.
 * Вместо одного прохода по массиву мы делаем несколько проходов с множественными 
 * операциями деления. Это позволяет показать реальное преимущество SIMD-инструкций,
 * так как время вычислений начинает доминировать над временем работы с памятью.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h> // Заголовочный файл для инструкций SIMD
#include <random>
#include <iomanip>

// ============================================================================
// Скалярная версия с повышенной вычислительной интенсивностью
// ============================================================================
void scalar_division_intensive(const std::vector<double>& a, 
                                const std::vector<double>& b, 
                                std::vector<double>& result,
                                int iterations) {
    // Выполняем несколько итераций вычислений для увеличения нагрузки
    for (int iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < a.size(); ++i) {
            // Три последовательные операции деления
            result[i] = a[i] / b[i];                    // Первое деление
            result[i] = result[i] / (b[i] + 1.0);       // Второе деление
            result[i] = result[i] / (a[i] + 2.0);       // Третье деление
        }
    }
}

// ============================================================================
// SIMD-версия с повышенной вычислительной интенсивностью
// ============================================================================
void simd_division_intensive(const std::vector<double>& a, 
                              const std::vector<double>& b, 
                              std::vector<double>& result,
                              int iterations) {
    // Подготавливаем константы для SIMD-операций
    __m128d one = _mm_set1_pd(1.0);  // Вектор из двух значений 1.0
    __m128d two = _mm_set1_pd(2.0);  // Вектор из двух значений 2.0
    
    // Выполняем несколько итераций вычислений
    for (int iter = 0; iter < iterations; ++iter) {
        // Обрабатываем по 2 double за раз (128 бит)
        for (size_t i = 0; i < a.size(); i += 2) {
            // Загружаем данные в SIMD-регистры
            __m128d a_vec = _mm_loadu_pd(&a[i]);
            __m128d b_vec = _mm_loadu_pd(&b[i]);
            __m128d result_vec = _mm_loadu_pd(&result[i]);
            
            // Выполняем три последовательные операции деления параллельно над двумя элементами
            result_vec = _mm_div_pd(a_vec, b_vec);                      // Первое деление
            result_vec = _mm_div_pd(result_vec, _mm_add_pd(b_vec, one)); // Второе деление
            result_vec = _mm_div_pd(result_vec, _mm_add_pd(a_vec, two)); // Третье деление
            
            // Сохраняем результат обратно в память
            _mm_storeu_pd(&result[i], result_vec);
        }
    }
}

// ============================================================================
// Функция для многократного измерения времени выполнения (для точности)
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
    
    return total_time / rounds; // Возвращаем среднее время
}

// ============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================================
int main() {
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  УЛУЧШЕННАЯ ДЕМОНСТРАЦИЯ ПРОИЗВОДИТЕЛЬНОСТИ SIMD  ║" << std::endl;
    std::cout << "║  Студент № 58: Инструкция _mm_div_pd              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // Конфигурация параметров теста
    // ========================================================================
    const size_t ARRAY_SIZE = 10000000;      // 10 миллионов элементов
    const int ITERATIONS = 10;                // Количество итераций для интенсивных вычислений
    const int BENCHMARK_ROUNDS = 3;           // Количество повторений теста для усреднения

    std::cout << "Параметры теста:" << std::endl;
    std::cout << "  Размер массива:     " << ARRAY_SIZE << " элементов" << std::endl;
    std::cout << "  Итераций расчета:   " << ITERATIONS << std::endl;
    std::cout << "  Раундов измерений:  " << BENCHMARK_ROUNDS << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // Инициализация данных
    // ========================================================================
    std::vector<double> dividends(ARRAY_SIZE);
    std::vector<double> divisors(ARRAY_SIZE);
    std::vector<double> result_scalar(ARRAY_SIZE);
    std::vector<double> result_simd(ARRAY_SIZE);

    std::random_device rd;
    std::mt19937 gen(42); // Фиксированное начальное значение для воспроизводимости
    std::uniform_real_distribution<> dis(1.0, 100.0);

    std::cout << "Инициализация массивов..." << std::endl;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        dividends[i] = dis(gen) * 5.0;  // Делимое: от 5 до 500
        divisors[i] = dis(gen);         // Делитель: от 1 до 100
    }
    std::cout << "Инициализация завершена!" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ
    // ========================================================================
    std::cout << "┌───────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│  ИНТЕНСИВНЫЕ ВЫЧИСЛЕНИЯ (многократное деление)    │" << std::endl;
    std::cout << "└───────────────────────────────────────────────────┘" << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    // Тестирование скалярной версии
    std::cout << "Запуск скалярной версии..." << std::endl;
    double scalar_time = benchmark([&]() {
        scalar_division_intensive(dividends, divisors, result_scalar, ITERATIONS);
    }, BENCHMARK_ROUNDS);

    std::cout << "  ✓ Скалярная версия завершена" << std::endl;
    std::cout << "  Среднее время: " << scalar_time << " мс" << std::endl;
    std::cout << std::endl;

    // Тестирование SIMD-версии
    std::cout << "Запуск SIMD-версии..." << std::endl;
    double simd_time = benchmark([&]() {
        simd_division_intensive(dividends, divisors, result_simd, ITERATIONS);
    }, BENCHMARK_ROUNDS);

    std::cout << "  ✓ SIMD-версия завершена" << std::endl;
    std::cout << "  Среднее время: " << simd_time << " мс" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // РЕЗУЛЬТАТЫ
    // ========================================================================
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  РЕЗУЛЬТАТЫ ИЗМЕРЕНИЙ                             ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  Скалярная версия (цикл):  " << std::setw(10) << scalar_time << " мс      ║" << std::endl;
    std::cout << "║  SIMD версия (_mm_div_pd): " << std::setw(10) << simd_time << " мс      ║" << std::endl;
    std::cout << "║  ───────────────────────────────────────────────  ║" << std::endl;

    double speedup = scalar_time / simd_time;
    std::cout << "║  Ускорение:                " << std::setw(10) << speedup << " раз     ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================================================
    // ПРОВЕРКА КОРРЕКТНОСТИ
    // ========================================================================
    std::cout << "Проверка корректности результатов..." << std::endl;
    
    // Запускаем оба алгоритма еще раз для проверки
    scalar_division_intensive(dividends, divisors, result_scalar, 1);
    simd_division_intensive(dividends, divisors, result_simd, 1);
    
    bool correct = true;
    double max_error = 0.0;
    
    for (size_t i = 0; i < std::min(size_t(1000), ARRAY_SIZE); ++i) {
        double error = std::abs(result_scalar[i] - result_simd[i]);
        max_error = std::max(max_error, error);
        
        if (error > 1e-9) {
            std::cerr << "✗ ОШИБКА: несовпадение результатов на индексе " << i << std::endl;
            std::cerr << "  Скалярное значение: " << result_scalar[i] << std::endl;
            std::cerr << "  SIMD значение:      " << result_simd[i] << std::endl;
            std::cerr << "  Разница:            " << error << std::endl;
            correct = false;
            break;
        }
    }
    
    if (correct) {
        std::cout << "✓ Проверка пройдена успешно!" << std::endl;
        std::cout << "  Максимальная погрешность: " << std::scientific << max_error << std::fixed << std::endl;
    }
    std::cout << std::endl;

    // ========================================================================
    // ЗАКЛЮЧЕНИЕ
    // ========================================================================
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ЗАКЛЮЧЕНИЕ                                       ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  Инструкция _mm_div_pd обрабатывает одновременно  ║" << std::endl;
    std::cout << "║  2 значения типа double, что обеспечивает         ║" << std::endl;
    std::cout << "║  значительное ускорение при вычислительно-        ║" << std::endl;
    std::cout << "║  интенсивных операциях.                           ║" << std::endl;
    std::cout << "║                                                   ║" << std::endl;
    std::cout << "║  Увеличение вычислительной нагрузки позволило     ║" << std::endl;
    std::cout << "║  продемонстрировать реальное преимущество SIMD    ║" << std::endl;
    std::cout << "║  над скалярными вычислениями.                     ║" << std::endl;
    std::cout << "║                                                   ║" << std::endl;
    std::cout << "║  Для компиляции используйте:                      ║" << std::endl;
    std::cout << "║  make run_no_vec                                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;

    return correct ? 0 : 1;
}
