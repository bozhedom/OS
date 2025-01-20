#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file!");
        isValid = false;
        return;
    }
#ifdef _WIN32
    processId = GetCurrentProcessId();
#else
    processId = getpid();
#endif
    isValid = true;
}

Logger::~Logger() {
    isValid = false;
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::logStartProcess() {
    if (!isValid) return;

    #ifdef _WIN32
    SYSTEMTIME st;
    GetSystemTime(&st);
    logFile << "Process ID: " << processId << ", Start time: "
            << st.wDay << "/" << st.wMonth << "/" << st.wYear << " "
            << 10 + st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" 
            << st.wMilliseconds << std::endl;
#else
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&now_c);
    logFile << "Process ID: " << processId << ", Start time: "
            << std::put_time(tm, "%d/%m/%Y %H:%M:%S") << std::endl;
#endif
}

void Logger::logMessage(const std::string& message, int counter) {
    if (!isValid) return;

#ifdef _WIN32
    SYSTEMTIME st;
    GetSystemTime(&st);
    logFile << "Time: "
            << st.wDay << "/" << st.wMonth << "/" << st.wYear << " "
            << 10 + st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" 
            << st.wMilliseconds << ", Process ID: " << processId
            << ", " << message << ": " << counter << std::endl;
#else
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&now_c);
    logFile << "Time: "
            << std::put_time(tm, "%d/%m/%Y %H:%M:%S") << ", Process ID: " << processId
            << ", " << message << ": " << counter << std::endl;
#endif
}

bool Logger::isValidLogger() const {
    return isValid;     
}