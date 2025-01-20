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

const std::string counterFilePath = "counter.txt";

std::mutex counterMutex;
std::mutex fileMutex;

// Функция для чтения текущего значения счетчика из файла
int getCounterValue() {
    std::lock_guard<std::mutex> lock(fileMutex); 
    std::ifstream file(counterFilePath);
    int counterValue = 0;

    if (file.is_open()) {
        file >> counterValue;
    } else {
        std::cerr << "Error: Unable to open counter file." << std::endl;
    }

    return counterValue;
}

// Функция для записи нового значения счетчика в файл
void setCounterValue(int value) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream file(counterFilePath);
    if (file.is_open()) {
        file << value;
    } else {
        std::cerr << "Error: Unable to write to counter file." << std::endl;
    }
}

// Функция для инкрементации счетчика
void incrementCounter(int value) {
    int currentValue = getCounterValue();
    setCounterValue(currentValue + value);
}

// Функция для логирования каждую секунду
void logCounterEverySecond(Logger& logger) {
    int elapsedMs = 0;
    while (running) {
        Timer::sleepMilliseconds(300);
        elapsedMs += 300;

        if (elapsedMs >= 1000) {
            int counterValue = getCounterValue();
            logger.logMessage("Counter value", counterValue);
            elapsedMs = 0;
        }
    }
}

// Функция для инкремента счетчика раз в 300 мс
void incrementCounterEvery300ms() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        incrementCounter(1);
    }
}

// Функция для запуска копий (процессов)
void spawnCopies(Logger& logger) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        if (!copy1Running.load()) {
            copy1Running.store(true);
            std::thread copy1([&]() {
                std::lock_guard<std::mutex> lock(counterMutex);
                logger.logMessage("Copy 1 started, PID: " + std::to_string(getpid()), getCounterValue());
                incrementCounter(10);
                logger.logMessage("Copy 1 finished, PID: " + std::to_string(getpid()), getCounterValue());
                copy1Running.store(false);
            });
            copy1.detach();
        } else {
            logger.logMessage("Copy 1 already running, skipping.", getCounterValue());
        }

        if (!copy2Running.load()) {
            copy2Running.store(true);
            std::thread copy2([&]() {
                std::lock_guard<std::mutex> lock(counterMutex);
                logger.logMessage("Copy 2 started, PID: " + std::to_string(getpid()), getCounterValue());
                setCounterValue(getCounterValue() * 2);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                setCounterValue(getCounterValue() / 2);
                logger.logMessage("Copy 2 finished, PID: " + std::to_string(getpid()), getCounterValue());
                copy2Running.store(false);
            });
            copy2.detach();
        } else {
            logger.logMessage("Copy 2 already running, skipping.", getCounterValue());
        }
    }
}

// Функция для пользовательского ввода
void handleUserInput(Logger& logger) {
    while (running) {
        std::cout << "Enter new counter value (or press Enter to skip): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            int newValue = std::stoi(input);
            setCounterValue(newValue);
            std::cout << "Counter updated to: " << newValue << std::endl;
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
        if (access(counterFilePath.c_str(), F_OK) == -1) {
            // Если файла счетчика нет, создаем его с начальным значением
            std::ofstream file(counterFilePath);
            file << 0;  // Начальное значение счетчика
        }

        // --- Пункт 3: Управление счетчиком через командную строку ---
        std::cout << "Initial counter value: " << getCounterValue() << std::endl;

        // --- Пункт 4: Логирование и отсчет через потоки ---
        if (isMainProgram) {
            logger.logMessage("Counter started with value", getCounterValue());
            
            std::thread logThread(logCounterEverySecond, std::ref(logger));

            std::thread incrementThread(incrementCounterEvery300ms);

            std::thread spawnCopiesThread(spawnCopies, std::ref(logger));

            handleUserInput(logger);

            running = false;
            logThread.join();
            incrementThread.join();
            spawnCopiesThread.join();

            std::remove(flagFilePath.c_str());
        } else {
            // Если программа не главная, просто инкрементируем счетчик и ждем ввода
            std::thread incrementThread(incrementCounterEvery300ms);
            handleUserInput(logger);
            incrementThread.join();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
