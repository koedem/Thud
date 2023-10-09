#pragma once

#include <chrono>

class Timer {
private:
    using clock = std::chrono::high_resolution_clock;

    clock::time_point start;

public:
    Timer() {
        reset();
    }

    void reset() {
        start = clock::now();
    }

    [[nodiscard]] int64_t elapsed() const {
        auto end = clock::now();
        std::chrono::duration<double> duration = end - start;
        int64_t microseconds = std::chrono::duration_cast< std::chrono::microseconds >( duration ).count();
        return microseconds;
    }
};