#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class Logger {
public:
    explicit Logger(const std::string& filename);
    ~Logger();
    
    void logStartProcess();
    void logMessage(const std::string& message, int counter);

    bool isValidLogger() const;
private:
    std::ofstream logFile;
#ifdef _WIN32
    DWORD processId;
#else
    pid_t processId;
#endif

    bool isValid;
};

#endif // LOGGER_H
