#ifndef DATABASE_CONTROLLER_HSE_CALCULATOR_H
#define DATABASE_CONTROLLER_HSE_CALCULATOR_H

#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include <unordered_map>
#include "../types.h"
#include <type_traits>
#include <stdexcept>
#include <ctime>
#include <fstream>
#include <sstream>

enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

namespace database {
    class Logger {
    public:
        // Constructor: Opens the log file in append mode
        Logger()
        {
            std::ofstream logFile("log.txt");
            if (!logFile.is_open()) {
                std::cerr << "Error opening log file." << std::endl;
            }
        }

        // Destructor: Closes the log file
        ~Logger() { logFile.close(); }

        // Logs a message with a given log level
        void log(LogLevel level, const std::string& message);

    private:
        std::ofstream logFile; // File stream for the log file

        // Converts log level to a string for output
        std::string levelToString(LogLevel level)
        {
            switch (level) {
            case DEBUG:
                return "DEBUG";
            case INFO:
                return "INFO";
            case WARNING:
                return "WARNING";
            case ERROR:
                return "ERROR";
            case CRITICAL:
                return "CRITICAL";
            default:
                return "UNKNOWN";
            }
        }
    };
}




#endif //DATABASE_CONTROLLER_HSE_CALCULATOR_H