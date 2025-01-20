#include "Counter.h"

Counter::Counter(int initialValue) : value(initialValue) {}

void Counter::increment(int delta) {
    value.fetch_add(delta, std::memory_order_relaxed);
}

void Counter::setValue(int newValue) {
    value.store(newValue, std::memory_order_relaxed);
}

int Counter::getValue() const {
    return value.load(std::memory_order_relaxed);
}
