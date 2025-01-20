#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <mutex>
#include <fstream>
#include "Logger.h"
#include "Counter.h"
#include "Timer.h"

std::atomic<bool> running(true);
std::atomic<bool> copy1Running(false);
std::atomic<bool> copy2Running(false);

const std::string flagFilePath = "flag.txt";

std::mutex counterMutex;

// Функция для логирования каждую секунду
void logCounterEverySecond(Logger& logger, Counter& counter) {
    int elapsedMs = 0;
    while (running) {
        Timer::sleepMilliseconds(300);
        elapsedMs += 300;

        if (elapsedMs >= 1000) {
            std::lock_guard<std::mutex> lock(counterMutex);
            logger.logMessage("Counter value", counter.getValue());
            elapsedMs = 0;
        }
    }
}

// Функция для инкремента счетчика раз в 300 мс
void incrementCounterEvery300ms(Counter& counter) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::lock_guard<std::mutex> lock(counterMutex);
        counter.increment();
    }
}

// Функция для запуска копий (процессов)
void spawnCopies(Logger& logger, Counter& counter) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        if (!copy1Running.load()) {
            copy1Running.store(true);
            std::thread copy1([&]() {
                std::lock_guard<std::mutex> lock(counterMutex);
                logger.logMessage("Copy 1 started, PID: " + std::to_string(getpid()), counter.getValue());
                counter.setValue(10 + counter.getValue());
                logger.logMessage("Copy 1 finished, PID: " + std::to_string(getpid()), counter.getValue());
                copy1Running.store(false);
            });
            copy1.detach();
        } else {
            logger.logMessage("Copy 1 already running, skipping.", counter.getValue());
        }

        if (!copy2Running.load()) {
            copy2Running.store(true);
            std::thread copy2([&]() {
                std::lock_guard<std::mutex> lock(counterMutex);
                logger.logMessage("Copy 2 started, PID: " + std::to_string(getpid()), counter.getValue());
                counter.setValue(counter.getValue() * 2);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                counter.setValue(counter.getValue() / 2);
                logger.logMessage("Copy 2 finished, PID: " + std::to_string(getpid()), counter.getValue());
                copy2Running.store(false);
            });
            copy2.detach();
        } else {
            logger.logMessage("Copy 2 already running, skipping.", counter.getValue());
        }
    }
}

// Функция для пользовательского ввода
void handleUserInput(Logger& logger, Counter& counter) {
    while (running) {
        std::cout << "Enter new counter value (or press Enter to skip): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            std::lock_guard<std::mutex> lock(counterMutex);
            counter.setValue(std::stoi(input));
            std::cout << "Counter updated to: " << counter.getValue() << std::endl;

            if (logger.isValidLogger()) {
                logger.logMessage("Counter updated to", counter.getValue());
            }
            
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        // --- Пункт 1: Открытие лог-файла и запись информации о процессе ---
        Logger logger("log.txt");

        // Проверяем, существует ли файл-флаг, который определяет главную программу
        bool isMainProgram = false;
        if (access(flagFilePath.c_str(), F_OK) == -1) {
            // Если файл не существует, программа становится главной
            std::ofstream file(flagFilePath);
            isMainProgram = true;
            
            logger.logStartProcess();
        }

        // --- Пункт 2: Инициализация счетчика с значением из аргумента или по умолчанию ---
        int initialCounter = 0;
        if (argc > 1) {
            initialCounter = std::stoi(argv[1]);
        }
        Counter counter(initialCounter);

        // --- Пункт 3: Управление счетчиком через командную строку ---
        std::cout << "Initial counter value: " << counter.getValue() << std::endl;

        // --- Пункт 4: Логирование и отсчет через потоки ---
        if (isMainProgram) {
            logger.logMessage("Counter started with value", counter.getValue());
            
            std::thread logThread(logCounterEverySecond, std::ref(logger), std::ref(counter));

            std::thread incrementThread(incrementCounterEvery300ms, std::ref(counter));

            std::thread spawnCopiesThread(spawnCopies, std::ref(logger), std::ref(counter));

            handleUserInput(logger, counter);

            running = false;
            logThread.join();
            incrementThread.join();
            spawnCopiesThread.join();

            std::remove(flagFilePath.c_str());
        } else {
            // Если программа не главная, просто инкрементируем счетчик и ждем ввода
            std::thread incrementThread(incrementCounterEvery300ms, std::ref(counter));
            handleUserInput(logger, counter);
            incrementThread.join();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
