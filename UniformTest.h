#pragma once
#include "TestDistribution.h"
#include <random>
#ifdef _OPENMP
#include <omp.h>
#endif

template<typename T>
class UniformTest : public TestDistribution<T> {
public:
    UniformTest(int n, T min, T max, bool use_parallel = false)
        : TestDistribution<T>(n)
    {
        if (max <= min) throw std::invalid_argument("max must be greater than min");

        std::uniform_real_distribution<double> dist(
            static_cast<double>(min), static_cast<double>(max));

#ifdef _OPENMP
        if (use_parallel) {
            const int threads = omp_get_max_threads();
            std::vector<std::mt19937> generators;
            generators.reserve(threads);
            std::random_device rd;
            for (int i = 0; i < threads; ++i) {
                std::seed_seq seed{rd(), rd(), rd(), rd()};
                generators.emplace_back(seed);
            }
            #pragma omp parallel for
            for (int i = 0; i < n; ++i) {
                this->values[i] = static_cast<T>(dist(generators[omp_get_thread_num()]));
            }
            return;
        }
#else
        (void)use_parallel;
#endif
        std::random_device rd;
        std::mt19937 gen(rd());
        for (int i = 0; i < n; ++i)
            this->values[i] = static_cast<T>(dist(gen));
    }
};
