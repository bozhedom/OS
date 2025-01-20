#ifndef COUNTER_H
#define COUNTER_H

#include <atomic>

class Counter {
public:
    explicit Counter(int initialValue = 0);
    void increment(int delta = 1);
    void setValue(int value);
    int getValue() const;

private:
    std::atomic<int> value;  // Используем атомарную переменную
};

#endif // COUNTER_H
