#pragma once
#include "stm32f1xx_hal.h"

class LowPowerMode {
public:
    LowPowerMode() = default;
    LowPowerMode(const LowPowerMode&) = delete;
    LowPowerMode& operator=(const LowPowerMode&) = delete;

    void EnterStopMode() {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }

    void EnterStandbyMode() {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB | PWR_FLAG_PVDO);
        HAL_PWR_EnterSTANDBYMode();
    }
};