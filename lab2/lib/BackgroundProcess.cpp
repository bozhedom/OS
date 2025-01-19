#include "BackgroundProcess.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>

namespace BackgroundProcess {

#ifdef _WIN32

std::optional<ProcessHandle> startProcess(const std::string& program, const std::string& args) {
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(STARTUPINFO);

    std::string command = program + " " + args;
    if (!CreateProcess(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to start process.\n";
        return std::nullopt;
    }

    CloseHandle(pi.hThread); // Закрываем ненужный handle потока
    return pi.hProcess;      // Возвращаем handle процесса
}

bool waitProcess(ProcessHandle process, int timeoutSeconds) {
    DWORD waitResult = (timeoutSeconds < 0) ? 
        WaitForSingleObject(process, INFINITE) : 
        WaitForSingleObject(process, timeoutSeconds * 1000);

    return waitResult == WAIT_OBJECT_0;
}

std::optional<int> getExitCode(ProcessHandle process) {
    DWORD exitCode;
    if (!GetExitCodeProcess(process, &exitCode)) {
        std::cerr << "Failed to get exit code.\n";
        return std::nullopt;
    }
    CloseHandle(process); // Закрываем handle процесса
    return static_cast<int>(exitCode);
}

#else

std::optional<ProcessHandle> startProcess(const std::string& program, const std::string& args) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork process.\n";
        return std::nullopt;
    } else if (pid == 0) {
        if (execlp(program.c_str(), program.c_str(), args.c_str(), nullptr) == -1) {
            std::cerr << "Unknown command: " << program << "\n";
            _exit(127);
        }
    }
    return pid;
}

std::optional<int> waitProcess(ProcessHandle process, int timeoutSeconds) {
    int status;
    if (timeoutSeconds < 0) {
        if (waitpid(process, &status, 0) == -1) {
            std::cerr << "Error while waiting for process.\n";
            return std::nullopt;
        }
    } else {
        auto startTime = std::chrono::steady_clock::now();
        while (true) {
            int result = waitpid(process, &status, WNOHANG);
            if (result > 0) {
                if (WIFEXITED(status)) {
                    return WEXITSTATUS(status);
                } else {
                    std::cerr << "Process did not terminate normally.\n";
                    return std::nullopt;
                }
            };     
            if (result < 0) { 
                std::cerr << "Error while waiting for process.\n";
                return std::nullopt;
            }
            if (std::chrono::steady_clock::now() - startTime > std::chrono::seconds(timeoutSeconds)) {
                std::cerr << "Timeout exceeded while waiting for process.\n";
                return std::nullopt;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    std::cerr << "Process did not terminate normally.\n";
    return std::nullopt;
}

#endif

}
