#pragma once
#include "stm32f1xx_hal.h"
#include <vector>
class I2CChannel {
public:
    I2C_HandleTypeDef hi2c1;
    I2CChannel() = default;
    /**
     * @brief 构造函数，初始化I2C通道
     * @param hi2c 指向已配置好的I2C句柄
     * @details 外部需配置I2C核心参数（如时钟速度、地址模式等）
     */
    I2CChannel(I2C_HandleTypeDef hi2c)
    : hi2c1(hi2c) {
        HAL_I2C_Init(&hi2c1);
    };
     /**
     * @brief 向I2C从设备指定寄存器写入数据
     * @param dev_addr 从设备7位地址（无需左移，内部自动处理读写位）
     * @param reg_addr 从设备内部寄存器地址
     * @param bit 寄存器地址长度（默认8位，可选I2C_MEMADD_SIZE_16BIT）
     * @param data 待写入的数据缓冲区（需确保缓冲区有效）
     * @param len 写入数据的字节数
     * @param timeout 超时时间（ms，默认100ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     * @details 内部调用HAL_I2C_Mem_Write，自动完成"起始信号→地址→寄存器→数据→停止信号"流程
     */
    HAL_StatusTypeDef I2C_Write(
        uint8_t dev_addr,         // 必选：从设备7位地址
        uint16_t reg_addr,        // 必选：寄存器地址
        uint8_t* data,            // 必选：数据缓冲区
        uint16_t len,             // 必选：数据长度
        uint16_t bit = I2C_MEMADD_SIZE_8BIT,  // 可选：寄存器地址长度（默认8位）
        uint32_t timeout = 100    // 可选：超时时间（默认100ms）
    ) {
    return HAL_I2C_Mem_Write(&hi2c1, 
                            dev_addr << 1,  // 7位地址左移1位（最低位0表示写）
                            reg_addr,       // 目标寄存器地址
                            bit,  // 寄存器地址长度：8位
                            data,           // 待发送数据
                            len,            // 数据长度
                            timeout);           // 超时时间（ms）
    }
    
    HAL_StatusTypeDef I2C_Write( 
         uint8_t dev_addr,         // 必选：从设备7位地址
        uint16_t reg_addr,        // 必选：寄存器地址
        std::vector<uint8_t*>& data,            // 必选：数据缓冲区
        uint16_t len,             // 必选：数据长度
        uint16_t bit = I2C_MEMADD_SIZE_8BIT,  // 可选：寄存器地址长度（默认8位）
        uint32_t timeout = 100    // 可选：超时时间（默认100ms）
    ) {
        for(size_t i=0;i<data.size();++i){
            auto Status = HAL_I2C_Mem_Write(&hi2c1, 
                                dev_addr << 1,  // 7位地址左移1位（最低位0表示写）
                                reg_addr,       // 目标寄存器地址
                                bit,  // 寄存器地址长度：8位
                                data[i],           // 待发送数据
                                len,            // 数据长度
                                timeout);           // 超时时间（ms）
            if(Status !=HAL_OK){
                return Status;
            }
        }
        return HAL_OK;
    }
    
     HAL_StatusTypeDef I2C_Write(
        uint8_t dev_addr,
        uint16_t reg_addr,
        const std::vector<std::vector<uint8_t>>& data,  // 多数据块集合
        uint16_t bit = I2C_MEMADD_SIZE_8BIT,
        uint32_t timeout = 100
    ) {
        for (const auto& block : data) {
            HAL_StatusTypeDef status = I2C_Write(
                dev_addr,
                reg_addr,
                const_cast<uint8_t*>(block.data()),  // 数据块首地址
                block.size(),                        // 数据块长度（2字节）
                bit,
                timeout
            );
            if (status != HAL_OK) {
                return status;  // 任一数据块失败则返回错误
            }
        }
        return HAL_OK;
    }
    /**
     * @brief 从I2C从设备指定寄存器读取数据
     * @param dev_addr 从设备7位地址（无需左移，内部自动处理读写位）
     * @param reg_addr 从设备内部寄存器地址
     * @param bit 寄存器地址长度（默认8位，可选I2C_MEMADD_SIZE_16BIT）
     * @param data 接收数据的缓冲区（需提前分配内存）
     * @param len 读取数据的字节数
     * @param timeout 超时时间（ms，默认100ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     * @details 内部调用HAL_I2C_Mem_Read，自动完成"起始信号→写地址→寄存器→重起始→读地址→读数据→停止信号"流程
     */
    HAL_StatusTypeDef I2C_Read(
        uint8_t dev_addr,         // 必选：从设备7位地址
        uint8_t reg_addr,         // 必选：寄存器地址
        uint8_t* data,            // 必选：接收缓冲区
        uint16_t len,             // 必选：读取长度
        uint16_t bit = I2C_MEMADD_SIZE_8BIT,  // 可选：寄存器地址长度（默认8位）
        uint32_t timeout = 100    // 可选：超时时间（默认100ms）
    ) {
        return HAL_I2C_Mem_Read(&hi2c1, 
                                dev_addr << 1,  // 7位地址左移1位（最低位1表示读，内部自动处理）
                                reg_addr,       // 目标寄存器地址
                                bit,  // 寄存器地址长度：8位
                                data,           // 接收缓冲区
                                len,            // 读取长度
                                timeout);           // 超时时间（ms）
    }


};

class SPIChannel {
public:
    SPI_HandleTypeDef hspi1;
    //SPIChannel(const SPIChannel&) = delete;
    SPIChannel() = default;
    SPIChannel(SPI_HandleTypeDef hspi):hspi1(hspi){
                HAL_SPI_Init(&hspi1);

    }
    /**
     * @brief 通过SPI发送数据
     * @param data 待发送数据缓冲区
     * @param len 发送数据长度（字节数）
     * @param timeout 超时时间（ms，默认100ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     * @details 内部调用HAL_SPI_Transmit完成数据发送
     */
    HAL_StatusTypeDef SPI_SendData(uint8_t* data, uint16_t len,uint32_t timeout=100) {
        return HAL_SPI_Transmit(&hspi1, data, len, timeout); 
    }
        HAL_StatusTypeDef SPI_SendData(std::vector<uint8_t*> data, uint16_t len,uint32_t timeout=100) {
            for(size_t i=0;i<data.size();i++){
                 HAL_StatusTypeDef Status = HAL_SPI_Transmit(&hspi1, data[i], len, timeout); 
                if(Status!=HAL_OK){
                    return Status;
                }
            }
        return HAL_OK;
    }
    /**
     * @brief 通过SPI接收数据
     * @param data 接收数据缓冲区
     * @param len 接收数据长度（字节数）
     * @param timeout 超时时间（ms，默认100ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     * @details 内部调用HAL_SPI_Receive完成数据接收
     */
    HAL_StatusTypeDef SPI_ReceiveData(uint8_t* data, uint16_t len,uint32_t timeout=100) {
        return HAL_SPI_Receive(&hspi1, data, len, timeout); // 接收数据（超时100ms）
    }
    /**
     * @brief 通过SPI同时发送和接收数据
     * @param tx_data 待发送数据缓冲区
     * @param rx_data 接收数据缓冲区
     * @param len 发送和接收数据长度（字节数）
     * @param timeout 超时时间（ms，默认100ms）
     * @return HAL_StatusTypeDef HAL状态码（HAL_OK表示成功，其他为错误状态）
     * @details 内部调用HAL_SPI_TransmitReceive完成数据收发
     */
    HAL_StatusTypeDef SPI_TransmitReceive(uint8_t* tx_data, uint8_t* rx_data, uint16_t len,uint32_t timeout=100) {
        return  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, len, timeout);;
    }
};