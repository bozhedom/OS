#include "Logger.h"
#include <iostream>

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

    SYSTEMTIME st;
    GetSystemTime(&st);

    logFile << "Process ID: " << processId << ", Start time: "
            << st.wDay << "/" << st.wMonth << "/" << st.wYear << " "
            << 10 + st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":"
            << st.wMilliseconds << std::endl;
}

void Logger::logMessage(const std::string& message, int counter) {
    if (!isValid) return;

    SYSTEMTIME st;
    GetSystemTime(&st);

    logFile << "Time: "
            << st.wDay << "/" << st.wMonth << "/" << st.wYear << " "
            << 10 + st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":"
            << st.wMilliseconds << ", Process ID: " << processId
            << ", " << message << ": " << counter << std::endl;
}

bool Logger::isValidLogger() const {
    return isValid;     
}