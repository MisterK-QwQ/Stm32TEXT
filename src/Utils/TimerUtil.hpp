#pragma once
#include "stm32f1xx_hal.h"
#include "Utils/Logger.hpp"

class TimerUtil {
private:
    uint32_t lastMS;      // 上次记录的毫秒数
    uint32_t ms;          // 基础计时起点
    uint32_t prevMS;      // 差值计算起点
    uint32_t timeMS;      // 时间窗口起点

public:
    TimerUtil() noexcept 
        : lastMS(HAL_GetTick()), 
          ms(HAL_GetTick()), 
          prevMS(HAL_GetTick()), 
          timeMS(HAL_GetTick()) {}

    static uint32_t getCurrentMs() {
        return HAL_GetTick();
    }

    bool hasTimePassed(uint32_t ms) {
        return (getCurrentMs() - timeMS) >= ms;
    }

    uint32_t getTime() {
        return getCurrentMs();
    }

    void reset2() {
        prevMS = getTime();
    }

    uint32_t getDifference() {
        return getTime() - prevMS;
    }

    void setDifference(uint32_t difference) {
        prevMS = getTime() - difference;
    }

    void resetTime() {
        lastMS = getCurrentMs();
        timeMS = getCurrentMs();
    }

    uint32_t getElapsedTime1() {
        return getCurrentMs() - timeMS;
    }

    bool hasTimedElapsed(uint32_t time, bool reset = false) {  //ms
        if ((getCurrentMs() - lastMS) > time) {
            if (reset) {
                resetTime();
            }
            return true;
        }
        return false;
    }

    void reset1() {
        ms = getCurrentMs();
    }

    bool elapsed(uint32_t milliseconds) {
        return (getCurrentMs() - ms) > milliseconds;
    }

    uint32_t getElapsedTime() {
        return getCurrentMs() - ms;
    }
};