#include "Manager.hpp"

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
        if(data->initialized){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::read(GPIO_TypeDef port) {
     gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->initialized&&data->port==port){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::init() {
   /*  gpio.Add(GPIOA, 
         {GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}, 
         Hardware( 
             PWMChannel(
                 TIM_HandleTypeDef{
                     .Instance=TIM2,
                     {
                         .Prescaler=71, 
                         .CounterMode=TIM_COUNTERMODE_UP, 
                         .Period=100,
                         .ClockDivision=TIM_CLOCKDIVISION_DIV1,
                         .RepetitionCounter= 0,
                         .AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_DISABLE
                     },
                     .Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED,
                     .hdma={nullptr},
                     .Lock=HAL_UNLOCKED,
                     .State=HAL_TIM_STATE_RESET
                 },
                 TIM_OC_InitTypeDef{
                     .OCMode=TIM_OCMODE_PWM1,
                     .Pulse=0,
                     .OCPolarity=TIM_OCPOLARITY_HIGH,
                     .OCNPolarity=0, 
                     .OCFastMode=TIM_OCFAST_DISABLE,
                 },
                 TIM_CHANNEL_1
             ))); */
    
   // 在SPI句柄初始化处修改方向参数

    gpio.Add(GPIOC,{GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW});

   // gpio.Add(GPIOA, {GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // 按键输入
    gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_RX
    gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_TX

    I2C_HandleTypeDef hi2c1;
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0x00;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0x00;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    gpio.Add(GPIOB, {GPIO_PIN_8, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},Hardware(
        I2CChannel(hi2c1)));
    gpio.Add(GPIOB, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},Hardware(
        I2CChannel(hi2c1))); 

    gpio.InitAll();
    initManager=true;
}
Manager manager = Manager();
