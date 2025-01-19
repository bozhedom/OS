#ifndef BACKGROUND_PROCESS_H
#define BACKGROUND_PROCESS_H

#include <string>
#include <optional>

namespace BackgroundProcess {

#ifdef _WIN32
#include <windows.h>
using ProcessHandle = HANDLE;
#else
#include <sys/types.h>
using ProcessHandle = pid_t;
#endif

// Запускает программу в фоновом режиме
std::optional<ProcessHandle> startProcess(const std::string& program, const std::string& args);

// Ожидает завершения дочернего процесса
std::optional<int> waitProcess(ProcessHandle process, int timeoutSeconds = -1);

}

#endif // BACKGROUND_PROCESS_H
