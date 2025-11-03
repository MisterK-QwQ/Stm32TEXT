#pragma once
#include "stm32f1xx_hal.h"
#include <string>
#include <stdarg.h>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger{
public:
    Logger(UART_HandleTypeDef* huart):huart_(huart){};

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void Log( LogLevel level ,std::string message) {
#ifndef _Log
    return;
#endif
        std::string level_str = "";
        switch (level) {
            case LogLevel::DEBUG: level_str = "[DEBUG] "; break;
            case LogLevel::INFO:  level_str = "[INFO] "; break;
            case LogLevel::WARN:  level_str = "[WARN] "; break;
            case LogLevel::ERROR: level_str = "[ERROR] "; break;
        }
         HAL_UART_Transmit(huart_, (const uint8_t*)level_str.c_str(), level_str.length(), 100);
         HAL_UART_Transmit(huart_, (const uint8_t*)message.c_str(), message.length(), 100);
         HAL_UART_Transmit(huart_, (const uint8_t*)"\r\n", 2, 100);
    }

    void logF(LogLevel level ,std::string message, ...) {
#ifndef _Log
    return;
#endif
         std::string level_str = "";
        char buf[128];
         va_list args;
        va_start(args, message);
        vsnprintf(buf, sizeof(buf), message.c_str(), args);
        va_end(args);
        Log(level,buf);
    }

    
private:
     UART_HandleTypeDef* huart_;

};