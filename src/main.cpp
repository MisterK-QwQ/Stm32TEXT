#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"


uint32_t duty=0;
uint32_t Fduty=10;
void OnOpenLedEvent(GpioEvent& event) {
    if(event.pin==GPIO_PIN_0) {
    duty +=Fduty;
    if (duty >= 100) {
        Fduty = -10;
    }else if (duty == 0) {
        Fduty = 10;
    }
    HAL_Delay(100);
    //LogF.logF(LogLevel::INFO,"Duty:%d",duty);
    event.Data->hardware_info.pwm_channel.SetDuty(duty);
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
#ifdef _Log
    USART1_UART_Init();  //logger USART1初始化
#endif
    manager->init();
    manager->mDispatcher->registerListener<GpioEvent>(OnOpenLedEvent); // 注册事件监听器
    LogF.logF(LogLevel::INFO,"Initialized");
    while (true) {
        manager->read();     
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif

     //  LogF.logF(LogLevel::INFO,"Tick");
    }
}