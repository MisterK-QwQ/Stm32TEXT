#pragma once
#include "stm32f1xx_hal.h"
class UARTChannel {
public:
    UART_HandleTypeDef huart1;
    uint8_t rx_buf[128] = {0}; // 接收缓冲区
    uint16_t rx_len = 0; // 接收数据长度
UARTChannel() = default;
    /**
     * @brief 构造函数，初始化UART通道
     * @param huart 指向已配置好的UART句柄
     * @details 外部需配置UART核心参数（如波特率、数据位等）
     */
    UARTChannel(UART_HandleTypeDef huart): huart1(huart) {
        HAL_UART_Init(&huart1);
    };
    

    UARTChannel(const UARTChannel&) = delete;

    /**
     * @brief 发送数据
     * @param data 待发送数据缓冲区
     * @param len 发送数据长度
     * @param timeout 超时时间（ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     */
    HAL_StatusTypeDef UART_Send(uint8_t* data, uint16_t len, uint32_t timeout=100) {
        return HAL_UART_Transmit(&huart1, data, len, timeout);
    }

    /**
     * @brief 接收数据
     * @param data 接收数据缓冲区
     * @param len 接收数据长度
     * @param timeout 超时时间（ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     */
    HAL_StatusTypeDef UART_Receive(uint8_t* data, uint16_t len, uint32_t timeout=100) {
        return HAL_UART_Receive(&huart1, data, len, timeout);
    }
  /**
     * @brief DMA模式发送数据（无CPU干预）
     * @param data 待发送数据缓冲区
     * @param len 发送长度（字节）
     * @return HAL_StatusTypeDef HAL状态码
     */
    HAL_StatusTypeDef UART_Send_DMA(uint8_t* data, uint16_t len) {
        return HAL_UART_Transmit_DMA(&huart1, data, len);
    }

    /**
     * @brief DMA模式接收数据（无CPU干预）
     * @param data 接收缓冲区
     * @param len 接收长度（字节）
     * @return HAL_StatusTypeDef HAL状态码
     */
    HAL_StatusTypeDef UART_Receive_DMA(uint8_t* data, uint16_t len) {
        rx_len = len;
        return HAL_UART_Receive_DMA(&huart1, data, len);
    }

    /**
     * @brief 接收完成中断回调（需在USART中断中调用）
     */
    void UART_RxCompleteCallback() {
    }
};


