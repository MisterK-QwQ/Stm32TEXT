#include "Manager.hpp"
#include "Utils/Utils.hpp"

Manager::Manager(/* args */){
    //mDispatcher = EmbeddedEvent::Dispatcher(); 
}

Manager::~Manager(){
    if(gpio.clock[0].first){
        __HAL_RCC_GPIOA_CLK_DISABLE();
    }
     if(gpio.clock[1].first){
        __HAL_RCC_GPIOB_CLK_DISABLE();
    }
     if(gpio.clock[2].first){
        __HAL_RCC_GPIOC_CLK_DISABLE();
    }
}
void Manager::read() {
    gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->Gpio_initialized){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::read(GPIO_TypeDef port) {
     gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->Gpio_initialized&&data->port==port){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::init() {
    gpio.Add(GPIOA,{GPIO_PIN_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SDA
    gpio.Add(GPIOA,{GPIO_PIN_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SCL
    gpio.Add(GPIOA,{GPIO_PIN_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //CS
    gpio.Add(GPIOA,{GPIO_PIN_3,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //RES
    gpio.Add(GPIOA,{GPIO_PIN_2,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //A0
    gpio.Add(GPIOB, {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});    //背光A

    gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});   // USART1_TX
    gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});    // USART1_RX

    gpio.Add(GPIOC, {GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});  // 跳跃键
    gpio.InitAll();
    initManager=true;
}
Manager manager = Manager();
