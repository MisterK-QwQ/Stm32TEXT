#pragma once
#include "stm32f1xx.h"
class PWMChannel {
public:
    TIM_HandleTypeDef htim;
    uint32_t channel;
    TIM_OC_InitTypeDef sConfigOC;
    PWMChannel(const PWMChannel&) = delete;
    PWMChannel() = default;
    /**
     * @brief  构造函数，初始化PWM通道
     * @param tim 已配置好的定时器句柄
     * @param ch 定时器通道（如TIM_CHANNEL_1）
     * @note 调用此函数前请确保定时器已正确初始化并配置为PWM模式
     */
    PWMChannel(TIM_HandleTypeDef tim, TIM_OC_InitTypeDef sConfigO,uint32_t ch) {
        htim = tim;
        HAL_TIM_PWM_Init(&htim);
        channel = ch;
        sConfigOC = sConfigO;
        HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, channel);
    };
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
        sConfigOC.Pulse = duty;
        HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, channel);
        HAL_TIM_PWM_Start(&htim, channel);
    }
};