#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <windows.h>

class Logger {
public:
    explicit Logger(const std::string& filename);
    ~Logger();
    
    void logStartProcess();
    void logMessage(const std::string& message, int counter);

    bool isValidLogger() const;
private:
    std::ofstream logFile;
    DWORD processId;
    bool isValid;
};

#endif // LOGGER_H
