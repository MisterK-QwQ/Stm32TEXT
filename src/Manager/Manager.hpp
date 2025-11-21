#pragma once
#include "../Events/Event.hpp"
#include "DigitalCircuit/GPIO.hpp"
#include "../DigitalCircuit/HS12864TG10B.hpp"
class Manager{
public:
    HS12864TG10B LDC=HS12864TG10B(
        GPIOA, GPIO_PIN_5,  // SCL
        GPIOA, GPIO_PIN_7,  // SDA
        GPIOA, GPIO_PIN_2,  // A0
        GPIOA, GPIO_PIN_3,  // CS
        GPIOA, GPIO_PIN_4   // RES
    );

    Manager(/* args */);
    ~Manager();
    Manager(const Manager&)=default;
    Manager operator=(const Manager&);

    bool initManager=false;
    GPIO gpio;
    uint32_t tick=0;
   // UART_HandleTypeDef huart1;
    EmbeddedEvent::Dispatcher mDispatcher;

    void init();
    void read();
    void read(GPIO_TypeDef port); 
    


};
extern Manager manager;
