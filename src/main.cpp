#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"
#include "Manager/World.hpp"

uint8_t playerShape[24] = {
   0x1c,0x22,0x22,0x1c,0x08,0x1c,0x2a,0x2a,0x08,0x14,0x14,0x14
};


uint8_t obstacleShape[8] = {
    0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff
};
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
   // manager.LDC.showCustomChar(20,20,playerShape,12,9);
    GWorld.addEntity({20, (float)GWorld.groundY-12}, {8, 12}, Type::Character, playerShape);
    GWorld.addEntity({100, (float)GWorld.groundY - 4}, {4, 4}, Type::obstacle, obstacleShape);
   // GWorld.addEntity({100, (float)GWorld.groundY - 4}, {4, 4}, Type::obstacle, obstacleShape);

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
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
        for (auto& entity : GWorld.entities) {
            if (entity.type == Type::Character && entity.isOnGround) {
                entity.jump(180.0f);
                break;
            }
        }
    }
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