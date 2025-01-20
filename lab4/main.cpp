#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <cstring>

#ifdef _WIN32
#include <windows.h> 
#else
#include <fcntl.h>   
#include <unistd.h>  
#include <termios.h>  
#endif

// Чтение температуры с устройства через серийный порт
float readTemperatureFromSerial(const std::string& portName) {
#ifdef _WIN32
    std::wstring widePortName(portName.begin(), portName.end());

    HANDLE hSerial = CreateFileW(
        widePortName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка: не удалось открыть порт " << portName << std::endl;
        return -999.0f;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Ошибка: не удалось получить параметры порта" << std::endl;
        CloseHandle(hSerial);
        return -999.0f;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Ошибка: не удалось настроить порт" << std::endl;
        CloseHandle(hSerial);
        return -999.0f;
    }

    // Таймауты чтения
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    // Читаем данные
    char buffer[256] = { 0 };
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        std::cerr << "Ошибка: не удалось прочитать данные из порта" << std::endl;
        CloseHandle(hSerial);
        return -999.0f;
    }

    CloseHandle(hSerial);
    try {
        return std::stof(buffer); 
    } catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка: некорректный формат данных с устройства" << std::endl;
        return -999.0f;
    }

#else
    int serialPort = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (serialPort == -1) {
        std::cerr << "Ошибка: не удалось открыть порт " << portName << std::endl;
        return -999.0f; 
    }

    // Настраиваем параметры порта
    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Ошибка: не удалось получить атрибуты порта" << std::endl;
        close(serialPort);
        return -999.0f;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    tty.c_iflag &= ~IGNBRK;                     
    tty.c_lflag = 0;                           
    tty.c_oflag = 0;                           
    tty.c_cc[VMIN] = 1;                        
    tty.c_cc[VTIME] = 1;                      

    tcsetattr(serialPort, TCSANOW, &tty);

    // Буфер для данных
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    // Читаем данные из порта
    int bytesRead = read(serialPort, buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        close(serialPort);
        try {
            return std::stof(buffer);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка: некорректный формат данных с устройства" << std::endl;
            return -999.0f;
        }
    } else {
        std::cerr << "Ошибка: не удалось прочитать данные из порта" << std::endl;
        close(serialPort);
        return -999.0f;
    }
#endif
}

// Симуляция устройства для генерации температуры
float simulateDeviceTemperature() {
    // Генерируем случайную температуру от -10 до 35 градусов Цельсия
    return -10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (35.0f + 10.0f)));
}

// Получение текущего времени в строковом формате
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Получение текущей даты в формате YYYY-MM-DD
std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d");
    return oss.str();
}

// Запись строки в лог-файл
void writeLog(const std::string& filePath, const std::string& message) {
    std::ofstream logFile(filePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось открыть файл " << filePath << std::endl;
    }
}

// Функция для вычисления и записи средних значений температуры
void logHourlyAverage(const std::vector<float>& temperatures, const std::string& filePath) {
    if (!temperatures.empty()) {
        float sum = std::accumulate(temperatures.begin(), temperatures.end(), 0.0f);
        float avg = sum / temperatures.size();
        std::string timestamp = getCurrentTime();
        std::string logMessage = timestamp + " Hourly Average: " + std::to_string(avg);
        writeLog(filePath, logMessage);
    }
}

void logDailyAverage(const std::vector<float>& temperatures, const std::string& filePath) {
    if (!temperatures.empty()) {
        float sum = std::accumulate(temperatures.begin(), temperatures.end(), 0.0f);
        float avg = sum / temperatures.size();
        std::string timestamp = getCurrentTime();
        std::string logMessage = timestamp + " Daily Average: " + std::to_string(avg);
        writeLog(filePath, logMessage);
    }
}

// Удаление старых данных из лог-файла
void cleanOldData(const std::string& filePath, int hoursLimit) {
    std::ifstream inputFile(filePath);
    std::vector<std::string> lines;
    auto now = std::chrono::system_clock::now();

    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string timestamp;
            if (std::getline(iss, timestamp, ' ')) {
                std::tm timeStruct = {};
                std::istringstream ss(timestamp);
                ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
                auto logTime = std::chrono::system_clock::from_time_t(std::mktime(&timeStruct));

                auto duration = std::chrono::duration_cast<std::chrono::hours>(now - logTime).count();
                if (duration <= hoursLimit) {
                    lines.push_back(line);
                }
            }
        }
        inputFile.close();
    }

    // Перезаписываем файл с актуальными данными
    std::ofstream outputFile(filePath);
    if (outputFile.is_open()) {
        for (const auto& line : lines) {
            outputFile << line << std::endl;
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    // Пути к лог-файлам
    const std::string allMeasurementsLog = "all_measurements.log";
    const std::string hourlyAvgLog = "hourly_average.log";
    const std::string dailyAvgLog = "daily_average.log";

    #ifdef _WIN32
    const std::string serialPort = "COM3";
    #else
    const std::string serialPort = "/dev/ttyUSB0";
    #endif

    std::vector<float> hourlyTemperatures;
    std::vector<float> dailyTemperatures;

    // Цикл для периодического считывания температуры
    while (true) {
        float temperature = readTemperatureFromSerial(serialPort);

        if (temperature == -999.0f) {
            std::cerr << "Ошибка чтения с устройства. Используем симуляцию." << std::endl;
            temperature = simulateDeviceTemperature();
        }

        std::string timestamp = getCurrentTime();
        std::string logMessage = timestamp + " " + std::to_string(temperature);
        writeLog(allMeasurementsLog, logMessage);

        hourlyTemperatures.push_back(temperature);
        dailyTemperatures.push_back(temperature);

        // Логирование средней температуры каждый час
        if (hourlyTemperatures.size() >= 60) { // Примерно раз в час (60 минут)
            logHourlyAverage(hourlyTemperatures, hourlyAvgLog);
            hourlyTemperatures.clear();
        }

        // Логирование средней температуры каждый день
        if (dailyTemperatures.size() >= 1440) { // Примерно раз в день (1440 минут)
            logDailyAverage(dailyTemperatures, dailyAvgLog);
            dailyTemperatures.clear();
        }

        cleanOldData(allMeasurementsLog, 24);

        // Имитируем задержку 1 минута
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }

    return 0;
}

