#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

template<typename T>
class TestDistribution {
public:
    static constexpr int kHistogramBuckets = 20;

    explicit TestDistribution(int n) : values(n) {
        if (n <= 0) throw std::invalid_argument("sample count must be positive");
    }
    virtual ~TestDistribution() = default;

    T getMin() const {
        return *std::min_element(values.begin(), values.end());
    }

    T getMax() const {
        return *std::max_element(values.begin(), values.end());
    }

    T getMedian() const {
        std::vector<T> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        const size_t mid = sorted.size() / 2;
        return (sorted.size() % 2 == 0)
            ? static_cast<T>((sorted[mid - 1] + sorted[mid]) / 2.0)
            : sorted[mid];
    }

    T getNumberRange() const {
        return getMax() - getMin();
    }

    [[nodiscard]] std::map<int, int> getHistogram() const {
        std::map<int, int> histogram;
        const T minVal = getMin();
        const T maxVal = getMax();
        T bucketSize = (maxVal - minVal) / static_cast<T>(kHistogramBuckets);
        if (bucketSize == T{0}) bucketSize = T{1};

        for (const auto& val : values) {
            int bucket = static_cast<int>((val - minVal) / bucketSize);
            if (bucket >= kHistogramBuckets) bucket = kHistogramBuckets - 1;
            histogram[bucket]++;
        }
        return histogram;
    }

    const std::vector<T>& getValues() const { return values; }

protected:
    std::vector<T> values;
};
