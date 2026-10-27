#pragma once
#include <chrono>

// Простой секундомер для замера времени операций.
class Timer {
public:
    void start() { start_ = std::chrono::steady_clock::now(); }
    double elapsedMs() const {
        return std::chrono::duration<double, std::milli>(
                   std::chrono::steady_clock::now() - start_).count();
    }
private:
    std::chrono::steady_clock::time_point start_{};
};
