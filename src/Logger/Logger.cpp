#include "Logger.h"
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

namespace database {
    void Logger::log(LogLevel level, const std::string& message)
        {
            // Get current timestamp
            time_t now = time(0);
            tm* timeinfo = localtime(&now);
            char timestamp[20];
            strftime(timestamp, sizeof(timestamp),
                    "%Y-%m-%d %H:%M:%S", timeinfo);

            // Create log entry
            std::ostringstream logEntry;
            logEntry << "[" << timestamp << "] "
                    << levelToString(level) << ": " << message
                    << std::endl;

            // Output to console
            std::cout << logEntry.str();

            // Output to log file
            if (logFile.is_open()) {
                logFile << logEntry.str();
                logFile
                    .flush(); // Ensure immediate write to file
            }
        }
}


