#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer {
public:
    static void sleepMilliseconds(int ms);
};

#endif // TIMER_H
