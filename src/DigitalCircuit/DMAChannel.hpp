#pragma once
#include "stm32f1xx_hal.h"

class DMAChannel {
private:
    volatile uint8_t transfer_complete = 0;  // 传输完成标志位
public:
    DMA_HandleTypeDef hdma;  // DMA句柄，存储DMA配置信息
    DMAChannel() = default;
    DMAChannel(DMA_HandleTypeDef dma): hdma(dma) {
        HAL_DMA_Init(&hdma);
    };
    
    DMAChannel(const DMAChannel&) = delete;


    /**
     * @brief 启动DMA传输（轮询模式，无中断）
     * @param src 源地址（外设寄存器地址或内存地址）
     * @param dest 目标地址（内存地址或外设寄存器地址）
     * @param len 传输数据长度（单位：数据宽度对应的单位数，如半字则为16位计数）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功）
     * @note 修正点：HAL_DMA_Start需要传入完整参数（句柄+源地址+目标地址+长度），
     *       无需手动设置CPAR/CMAR/CNDTR寄存器（函数内部会自动设置）
     */
    HAL_StatusTypeDef DMA_Start(void* src, void* dest, uint16_t len) {
        transfer_complete = 0;  // 重置完成标志
        return HAL_DMA_Start(&hdma, (uint32_t)src, (uint32_t)dest, len);
    }

    /**
     * @brief 启动带中断的DMA传输
     * @param src 源地址
     * @param dest 目标地址
     * @param len 传输长度
     * @return HAL_StatusTypeDef HAL状态码
     * @note 修正点：HAL_DMA_Start_IT同样需要完整参数，内部自动配置寄存器并使能中断
     */
    HAL_StatusTypeDef DMA_Start_IT(void* src, void* dest, uint16_t len) {
        transfer_complete = 0;
        return HAL_DMA_Start_IT(&hdma, (uint32_t)src, (uint32_t)dest, len);
    }

    /**
     * @brief 停止DMA传输（终止当前所有操作）
     * @return HAL_StatusTypeDef HAL状态码
     * @note 修正点：HAL库中没有HAL_DMA_Stop，正确函数是HAL_DMA_Abort（终止传输）
     */
    HAL_StatusTypeDef DMA_Stop() {
        return HAL_DMA_Abort(&hdma);
    }

    /**
     * @brief 检查DMA传输是否完成
     * @return uint8_t 1-完成，0-未完成
     */
    uint8_t IsTransferComplete() {
        return transfer_complete;
    }

    /**
     * @brief DMA传输完成中断回调（需在DMA全局中断中调用）
     * @details 当DMA传输完成（非循环模式）时，由中断服务函数调用此方法
     */
    void DMA_CompleteCallback() {
        transfer_complete = 1;  // 标记传输完成
    }
};