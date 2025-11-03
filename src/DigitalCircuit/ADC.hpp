#pragma once
#include "stm32f1xx_hal.h"
class ADCChannel {
private:
    uint8_t calibrated = 0;
public:
    ADC_HandleTypeDef hadc;

    ADCChannel() = default;
    ADCChannel(const ADCChannel&) = delete;

      /**
     * @brief 初始化ADC通道
     * @param adc 指向已配置好的ADC句柄（含通道、采样时间等）
     * @details 外部需配置ADC核心参数（如扫描模式、连续转换等）
     */
    void ADC_Init(ADC_HandleTypeDef* adc,ADC_ChannelConfTypeDef sConfig) {
        hadc = *adc;
        calibrated = 0; // 初始未校准
        HAL_ADC_Init(&hadc);
        HAL_ADC_ConfigChannel(&hadc, &sConfig);
    }

    /**
     * @brief ADC校准（提高转换精度） 初始化后需执行一次
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示校准成功）
     * @details 建议上电后执行一次，校准完成后标志位置1
     */
    HAL_StatusTypeDef ADC_Calibrate() {    
        if (calibrated) return HAL_OK; // 已校准则直接返回
        HAL_StatusTypeDef status = HAL_ADCEx_Calibration_Start(&hadc);
        if (status == HAL_OK) calibrated = 1;
        return status;
    }

    /**
     * @brief 轮询模式单次转换（单通道）
     * @return uint32_t 转换结果（12位ADC对应0~4095）
     * @details 启动转换→等待完成→读取结果，阻塞式
     */
    uint32_t ADC_SingleConvert() {
        if (!calibrated) return 0; // 未校准则返回0
        HAL_ADC_Start(&hadc);                  // 启动ADC
        HAL_ADC_PollForConversion(&hadc, 100); // 等待转换完成（超时100ms）
        uint32_t val = HAL_ADC_GetValue(&hadc); // 读取结果
        HAL_ADC_Stop(&hadc);                   // 停止ADC
        return val;
    }

    /**
     * @brief DMA模式连续转换（支持多通道）
     * @param buf 存储转换结果的缓冲区
     * @param len 缓冲区长度（通道数或转换次数）
     * @return HAL_StatusTypeDef HAL状态码
     * @details 启动ADC+DMA，转换结果自动存入缓冲区（非阻塞）
     */
    HAL_StatusTypeDef ADC_Start_DMA(uint32_t* buf, uint16_t len) {
        if (!calibrated) return HAL_ERROR;
        return HAL_ADC_Start_DMA(&hadc, buf, len);
    }

    /**
     * @brief 停止DMA模式转换
     * @return HAL_StatusTypeDef HAL状态码
     */
    HAL_StatusTypeDef ADC_Stop_DMA() {
        return HAL_ADC_Stop_DMA(&hadc);
    }
};