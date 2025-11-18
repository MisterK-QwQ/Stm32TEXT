#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"
#include "Manager/World.hpp"
uint8_t ren[24]= {0x1c,0x00,0x22,0x00,0x1c,0x00,0x08,0x00,0x1c,0x00,0x2a,0x00,0x2a,0x00,0x08,0x00,0x14,0x00,0x14,0x00,0x14,0x00,0x14,0x00};

int main(void) {
    HAL_Init();
    SystemClock_Config();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
#ifdef _Log
    USART1_UART_Init();  //logger USART1初始化
#endif
    manager.init();
    LogF.logF(LogLevel::INFO,"Initialized");
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    manager.LDC.init();

    GWorld.addEntity({20,(float)GWorld.Ground+1},{2,8},Type::Character,ren);


    LogF.logF(LogLevel::INFO,"Gpio Size:%d GPIOA:%d GPIOB:%d GPIOC:%d"
        ,manager.gpio.GetGpioSize()
        ,manager.gpio.clock[0].second
        ,manager.gpio.clock[1].second
        ,manager.gpio.clock[2].second
    );

    TimerUtil time1;
    while (true) {
        manager.read();      
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif
    GWorld.upload();
    }
}
extern "C" void SysTick_Handler(void){   //每1msTick运行一次
  HAL_IncTick();  
  if(manager.initManager){
    manager.tick=HAL_GetTick();
  }
}

/* extern "C" void EXTI0_IRQHandler(void){
  
}
 */