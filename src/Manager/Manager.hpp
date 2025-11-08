#pragma once
#include "../Events/Event.hpp"
#include "DigitalCircuit/GPIO.hpp"

class Manager{
public:
    Manager(/* args */);
    ~Manager();
    Manager(const Manager&)=default;
    Manager operator=(const Manager&);

    bool initManager=false;
    GPIO gpio;

   // UART_HandleTypeDef huart1;
    EmbeddedEvent::Dispatcher mDispatcher;

    void init();
    void read();
    void read(GPIO_TypeDef port); 
    


};
extern Manager manager;
