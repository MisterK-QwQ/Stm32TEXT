#pragma once
#include "../include/Events.hpp"
#include "stm32f1xx_hal.h"
#include "DigitalCircuit/GPIO.hpp"

struct GpioEvent:EmbeddedEvent::Event{
    uint16_t pin;
    GPIO_TypeDef* Port;
    GPIO_PinState state;    
    GpioData* Data;
    bool Initialize=false;
    GpioEvent(uint16_t p, GPIO_TypeDef* pt, GPIO_PinState s,GpioData* data)
        : pin(p), Port(pt),state(s), Data(data){}
};
