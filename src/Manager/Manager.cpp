#include "Manager.hpp"


Manager::Manager(/* args */){
    mDispatcher = std::make_unique<EmbeddedEvent::Dispatcher>(); 
}

Manager::~Manager()
{
}
void Manager::read() {
    gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->initialized){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher->trigger(event);
        }
    });
}

void Manager::init() {
  
    // PA1作为TIM2_CH2输出PWM
   // gpio.Add(GPIOC, {GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});

    gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_RX
    gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_TX

    gpio.InitAll();
    
}
Manager* manager =new Manager();