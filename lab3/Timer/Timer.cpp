#include "Timer.h"

void Timer::sleepMilliseconds(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
