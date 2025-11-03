#pragma once
#include "stm32f1xx.h"

class PWMChannel {
public:
    TIM_HandleTypeDef htim;
    uint32_t channel;
    TIM_OC_InitTypeDef sConfigOC;
    /**
     * @brief  构造函数，初始化PWM通道
     * @param tim 已配置好的定时器句柄
     * @param ch 定时器通道（如TIM_CHANNEL_1）
     * @note 调用此函数前请确保定时器已正确初始化并配置为PWM模式
     */
    PWMChannel(TIM_HandleTypeDef tim, TIM_OC_InitTypeDef sConfigOC,uint32_t ch)
    : htim(tim), sConfigOC(sConfigOC), channel(ch) {
    HAL_TIM_PWM_Init(&htim);
        HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, channel);
    };
    PWMChannel(const PWMChannel&) = delete;
    /**
     * @brief 启动PWM输出
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     */
      HAL_StatusTypeDef PWM_Start() {
        return HAL_TIM_PWM_Start(&htim, channel);
    }
    /**
     * @brief 停止PWM输出
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     */
     HAL_StatusTypeDef PWM_Stop() {
        return HAL_TIM_PWM_Stop(&htim, channel);
    }

    /**
     * @brief 设置PWM占空比
     * @param duty 占空比，范围0-1000，对应0%-100%
     */
     void SetDuty(uint16_t duty) {
        if (duty > 1000) duty = 1000;
        uint32_t ccr = (uint32_t)(duty * htim.Init.Period) / 1000;
        switch(channel) {
            case TIM_CHANNEL_1: htim.Instance->CCR1 = ccr; break;
            case TIM_CHANNEL_2: htim.Instance->CCR2 = ccr; break;
            case TIM_CHANNEL_3: htim.Instance->CCR3 = ccr; break;
            case TIM_CHANNEL_4: htim.Instance->CCR4 = ccr; break;
            default: break;
        }
    }

};