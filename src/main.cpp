#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

void SysTick_Handler(void) {
  HAL_IncTick(); // 每1ms调用一次
}
int i=0;

void OnOpenLedEvent(GpioEvent& event) {
    if (event.pin == GPIO_PIN_0) {
        if(i<999)
            i=0;
        i+=50;

        event.Data->hardware_info.pwm_channel.SetDuty(i);
    }
}

int main(void) {
    if (HAL_Init() != HAL_OK) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitTypeDef init = {GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW};
        HAL_GPIO_Init(GPIOC, &init);
        while(1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for(volatile uint32_t i=0;i<100000;i++);
        }
    }
    
    if (SystemClock_Config() != true) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitTypeDef init = {GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW};
        HAL_GPIO_Init(GPIOC, &init);
        while(1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for(volatile uint32_t i=0;i<50000;i++);
        }
    }
    
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
    }
}