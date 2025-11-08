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
    
    Hardware() = default;

    Hardware(PWMChannel pwm){
        pwm_channel = pwm;
    }
    Hardware(SPIChannel spi) {
        spi_channel = spi;
    }
    Hardware(I2CChannel i2c){
        i2c_channel = i2c;
    }
    Hardware(DMAChannel dma){
        dma_channel = dma;
    }
    Hardware(ADCChannel adc){
        adc_channel = adc;
    }
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
   
    static constexpr size_t MAX_GPIO_PINS = 64;  // 最大支持64个引脚
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS> m_gpio_array;
    size_t m_count = 0;  // 当前已添加的引脚数量
public:
    std::array<std::pair<bool, size_t>, 12> clock = {{
            {false,0},//[0] GPIOA
            {false,0},//[1] GPIOB 
            {false,0}//[2] GPIOC
            //[...]
        }};

    GPIO() = default;
    ~GPIO() = default;

    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;

    size_t GetGpioSize(){
        return m_count;
    }
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS>& Get_gpio_array(){
        return m_gpio_array;
    }
    /**
     * @brief 添加GPIO引脚配置
     */
      void Add(GPIO_TypeDef* port, const GPIO_InitTypeDef& init, 
             const Hardware& hardware = Hardware()) {
        if (port == nullptr || m_count >= MAX_GPIO_PINS) return;  // 检查容量
        
        auto data = std::make_unique<GpioData>();
        data->port = port;
        data->init_config = init;
        data->hardware_info = hardware;
        data->initialized = false;
        m_gpio_array[m_count++] = std::move(data);  // 存入数组
    }

    /**
     * @brief 获取指定引脚的配置数据
     * @return 找到返回GpioData指针，否则返回nullptr
     */
     GpioData* GetData(GPIO_TypeDef* port, uint16_t pin) {
        if (port == nullptr || pin == 0) return nullptr;
        for (size_t i = 0; i < m_count; ++i) {
            auto& data = m_gpio_array[i];
            if (data && data->port == port && data->init_config.Pin == pin) {
                return data.get();
            }
        }
        return nullptr;
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
        for (size_t i = 0; i < m_count; ++i) {
            auto& data = m_gpio_array[i];
            if (data) {
                callback(data->port, data->init_config.Pin, data.get());
            }
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
        // 遍历数组中已添加的有效GPIO（仅遍历到m_count，避免多余操作）
        for (size_t i = 0; i < m_count; ++i) {
            const auto& data_ptr = m_gpio_array[i];
            if (!data_ptr) continue;  // 跳过空元素（理论上m_count内无空元素，保险起见）

            // 直接从GpioData中获取port和pin（无需分解key，效率更高）
            GPIO_TypeDef* port = data_ptr->port;
            uint16_t pin = data_ptr->init_config.Pin;

            // 检查条件是否匹配
            if (condition(port, pin, data_ptr.get())) {
                return {port, pin, data_ptr.get()};
            }
        }
        // 无匹配项
        return {nullptr, 0, nullptr};
    }

    /**
     * @brief 初始化所有未初始化的GPIO引脚
     */
    void InitAll() {
        ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
            if (!data->initialized) {
                if(data->port==GPIOA){
                    if(!clock[0].first){
                        __HAL_RCC_GPIOA_CLK_ENABLE();
                        clock[0].first=true;
                    }
                    ++clock[0].second;
                }
                
                if(data->port==GPIOB){
                      if(!clock[1].first){
                        __HAL_RCC_GPIOB_CLK_ENABLE();
                        clock[1].first=true;
                    }
                    ++clock[1].second;
                }
                
                if(data->port==GPIOC){
                    if(!clock[2].first){
                        __HAL_RCC_GPIOC_CLK_ENABLE();
                        clock[2].first=true;
                    }
                    ++clock[2].second;
                }
                
                HAL_GPIO_Init(data->port, &data->init_config);
                data->initialized = true;
            }
        });
    }
};