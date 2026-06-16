#pragma once
#include "TestDistribution.h"
#include <random>
#ifdef _OPENMP
#include <omp.h>
#endif

template<typename T>
class NormalTest : public TestDistribution<T> {
public:
    NormalTest(int n, T mean, T stddev, bool use_parallel = false)
        : TestDistribution<T>(n)
    {
        if (stddev <= T{0}) throw std::invalid_argument("stddev must be positive");

        std::normal_distribution<double> dist(
            static_cast<double>(mean), static_cast<double>(stddev));

#ifdef _OPENMP
        if (use_parallel) {
            const int threads = omp_get_max_threads();
            std::vector<std::mt19937> generators;
            generators.reserve(threads);
            std::vector<std::normal_distribution<double>> dists(threads, dist);
            std::random_device rd;
            for (int i = 0; i < threads; ++i) {
                std::seed_seq seed{rd(), rd(), rd(), rd()};
                generators.emplace_back(seed);
            }
            #pragma omp parallel for
            for (int i = 0; i < n; ++i) {
                const int tid = omp_get_thread_num();
                this->values[i] = static_cast<T>(dists[tid](generators[tid]));
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
