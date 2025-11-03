#pragma once
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <functional>  
#include "stm32f1xx_hal.h"

#include "DataChannel.hpp"
#include "PWMChannel.hpp"
#include "ADC.hpp"
#include "UARTChannel.hpp"
#include "DMAChannel.hpp"

using GpioKey = uint64_t;

inline GpioKey make_key(GPIO_TypeDef* port, uint16_t pin) {
    return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(port)) << 32) | pin;
}

inline void decompose_key(GpioKey key, GPIO_TypeDef*& port, uint16_t& pin) {
    uintptr_t port_ptr_val = static_cast<uintptr_t>(key >> 32);  // 高32位是端口指针
    port = reinterpret_cast<GPIO_TypeDef*>(port_ptr_val);
    pin = static_cast<uint16_t>(key & 0xFFFF);  // 低32位的低16位是引脚号
}

class Hardware {
public:
    PWMChannel pwm_channel;
    SPIChannel spi_channel;
    I2CChannel i2c_channel;
    DMAChannel dma_channel;
    ADCChannel adc_channel;
};

class GpioData {
public:
    bool initialized = false;               // 初始化状态标记
    GPIO_TypeDef* port = nullptr;           // GPIO端口指针
    GPIO_InitTypeDef init_config;           // GPIO初始化配置
    Hardware hardware_info;                 // 硬件相关信息
};

class GPIO {
private:
    std::unordered_map<GpioKey, std::unique_ptr<GpioData>> m_gpio_map;

public:
    GPIO() = default;
    ~GPIO() = default;

    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;

    /**
     * @brief 添加GPIO引脚配置
     */
    void Add(GPIO_TypeDef* port,const GPIO_InitTypeDef& init, 
             const Hardware& hardware = Hardware()) {
        if (port == nullptr) return;  // 无效参数检查
        GpioKey key = make_key(port, init.Pin);
        auto data = std::make_unique<GpioData>();
        data->port = port;
        data->init_config = init;
        data->hardware_info = hardware;
        data->initialized = false;
        m_gpio_map[key] = std::move(data);  // 使用move避免拷贝unique_ptr
    }

    /**
     * @brief 获取指定引脚的配置数据
     * @return 找到返回GpioData指针，否则返回nullptr
     */
    GpioData* GetData(GPIO_TypeDef* port, uint16_t pin) {
        if (port == nullptr || pin == 0) return nullptr;
        GpioKey key = make_key(port, pin);
        auto it = m_gpio_map.find(key);
        if (it != m_gpio_map.end()) {
            return it->second.get();  // 返回unique_ptr管理的原始指针
        }
        return nullptr;  // 修复原代码未返回的问题
    }

    /**
     * @brief 读取指定引脚电平
     */
    GPIO_PinState read(GPIO_TypeDef* port, uint16_t pin) {
        GpioData* data = GetData(port, pin);
        if (data && data->initialized && data->port != nullptr) {
            return HAL_GPIO_ReadPin(data->port, pin);
        }
        return GPIO_PIN_RESET;
    }
    
    /**
     * @brief 遍历所有已添加的GPIO引脚
     * @param callback 回调函数，参数为：端口、引脚号、GpioData指针
     */
    void ForEach(const std::function<void(GPIO_TypeDef*, uint16_t, GpioData*)>& callback) {
        for (const auto& [key, data_ptr] : m_gpio_map) {
            GPIO_TypeDef* port = nullptr;
            uint16_t pin = 0;
            decompose_key(key, port, pin);
            callback(port, pin, data_ptr.get());
        }
    }

    /**
     * @brief 遍历指定端口的所有引脚
     * @param port 目标端口（如GPIOA）
     * @param callback 回调函数，参数为：引脚号、GpioData指针
     */
    void ForEachInPort(GPIO_TypeDef* port, const std::function<void(uint16_t, GpioData*)>& callback) {
        if (port == nullptr) return;
        ForEach([&](GPIO_TypeDef* curr_port, uint16_t pin, GpioData* data) {
            if (curr_port == port) {
                callback(pin, data);
            }
        });
    }

    /**
     * @brief 按条件查找GPIO引脚
     * @param condition 条件函数（返回true表示匹配）
     * @return 第一个匹配的引脚数据（port, pin, data），无匹配则返回空
     */
    std::tuple<GPIO_TypeDef*, uint16_t, GpioData*> FindIf(
        const std::function<bool(GPIO_TypeDef*, uint16_t, GpioData*)>& condition
    ) {
        for (const auto& [key, data_ptr] : m_gpio_map) {
            GPIO_TypeDef* port = nullptr;
            uint16_t pin = 0;
            decompose_key(key, port, pin);
            if (condition(port, pin, data_ptr.get())) {
                return {port, pin, data_ptr.get()};
            }
        }
        return {nullptr, 0, nullptr};
    }

    /**
     * @brief 初始化所有未初始化的GPIO引脚
     */
    void InitAll() {
        ForEach([](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
            if (!data->initialized) {
                HAL_GPIO_Init(data->port, &data->init_config);
                data->initialized = true;
            }
        });
    }
};