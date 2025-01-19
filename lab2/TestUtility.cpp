#include "BackgroundProcess.h"
#include <iostream>
#include <string>

int main() {
    std::string program;
    std::string args;

    std::cout << "Enter the program to execute: ";
    std::cin >> program;
    std::cout << "Enter the arguments (or leave empty): ";
    std::cin.ignore();
    std::getline(std::cin, args);

    auto process = BackgroundProcess::startProcess(program, args);
    if (!process) {
        std::cerr << "Failed to start process.\n";
        return 1;
    }

    std::cout << "Process started. Waiting for it to finish...\n";

    auto exitCode = BackgroundProcess::waitProcess(*process, 10); // Таймаут 10 секунд
    if (!exitCode) {
        std::cerr << "Process did not finish in time or failed.\n";
        return 1;
    }

    std::cout << "Process finished with exit code: " << *exitCode << "\n";
    return 0;
}
