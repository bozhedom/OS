#include "BackgroundProcess.h"
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace BackgroundProcess {

#ifdef _WIN32

std::optional<ProcessHandle> startProcess(const std::string& program, const std::string& args) {
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(STARTUPINFOA);

    //"cmd.exe /C \"" для использования echo
    std::string command ="cmd.exe /C \"" + program + " " + args;

    if (!CreateProcessA(
            nullptr,                     
            command.data(),              
            nullptr,                     
            nullptr,                     
            FALSE,                       
            CREATE_NO_WINDOW,            
            nullptr,                    
            nullptr,                    
            &si,                         
            &pi)) {
        DWORD errorCode = GetLastError();
        if (errorCode == ERROR_FILE_NOT_FOUND) {
            std::cerr << "Unknown command: " << program << "\n";
        } else {
            std::cerr << "Failed to start process. Error code: " << errorCode << "\n";
        }
        return std::nullopt;
    }

    CloseHandle(pi.hThread);
    return pi.hProcess;
}

std::optional<int> waitProcess(ProcessHandle process, int timeoutSeconds) {
    DWORD waitResult;
    if (timeoutSeconds < 0) {
        waitResult = WaitForSingleObject(process, INFINITE);
    } else {
        waitResult = WaitForSingleObject(process, timeoutSeconds * 1000);
    }

    if (waitResult == WAIT_OBJECT_0) {
        DWORD exitCode;
        if (GetExitCodeProcess(process, &exitCode)) {
            CloseHandle(process);
            return static_cast<int>(exitCode);
        } else {
            std::cerr << "Failed to get exit code: " << GetLastError() << "\n";
            CloseHandle(process);
            return std::nullopt;
        }
    } else if (waitResult == WAIT_TIMEOUT) {
        std::cerr << "Timeout exceeded while waiting for process.\n";
        return std::nullopt;
    } else {
        std::cerr << "Error while waiting for process: " << GetLastError() << "\n";
        return std::nullopt;
    }
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
