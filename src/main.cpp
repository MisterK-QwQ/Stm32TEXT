#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

void OnOpenLedEvent(GpioEvent& event) {
    if(event.pin==GPIO_PIN_8||event.pin==GPIO_PIN_9){
        if(!event.Initialize){
            auto status = event.Data->hardware_info.i2c_channel.I2C_Write(
                0x3C,  
                0x00,
                { 
                    {0x00, 0xAE},  // 0x00=命令标志，0xAE=关闭显示
                    {0x00, 0xAF}   // 0x00=命令标志，0xAF=开启显示
                }
            );
            event.Initialize=true;
            LogF.logF(LogLevel::INFO, "OLED init status: %d", status);
        }
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_I2C1_CLK_ENABLE();
   // __HAL_RCC_ADC1_CLK_ENABLE();  
    // __HAL_RCC_I2C1_CLK_ENABLE();
   // __HAL_RCC_TIM3_CLK_ENABLE();  // 使能TIM3时钟
    //__HAL_RCC_TIM2_CLK_ENABLE();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
#ifdef _Log
    USART1_UART_Init();  //logger USART1初始化
#endif
    manager.init();
    manager.mDispatcher.registerListener<GpioEvent>(OnOpenLedEvent); // 注册事件监听器
    LogF.logF(LogLevel::INFO,"Initialized");
    LogF.logF(LogLevel::INFO,"Gpio Size:%d GPIOA:%d GPIOB:%d GPIOC:%d"
        ,manager.gpio.GetGpioSize()
        ,manager.gpio.clock[0].second
        ,manager.gpio.clock[1].second
        ,manager.gpio.clock[2].second
    );

    while (true) {
        manager.read();      
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif
    }
}
extern "C" void SysTick_Handler(void){   //每1msTick运行一次
  HAL_IncTick();  
  if(manager.initManager){

  }
}

/* extern "C" void EXTI0_IRQHandler(void){
  
}
 */