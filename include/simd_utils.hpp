#pragma once
#include <iostream>
#include <iomanip>

inline void print_vector(const double* v, size_t n) {
    std::cout << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < n; ++i)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}
