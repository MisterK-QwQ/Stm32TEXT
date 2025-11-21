#pragma once
#include "stm32f1xx_hal.h"
#include "DigitalCircuit/GPIO.hpp"  // 引入SPIChannel定义

#define LCD_WIDTH  128
#define LCD_HEIGHT 64
#define LCD_PAGE   8

class HS12864TG10B {
public:
    HS12864TG10B(GPIO_TypeDef* a0_port,  uint16_t a0_pin,
                 GPIO_TypeDef* cs_port,  uint16_t cs_pin,
                 GPIO_TypeDef* res_port, uint16_t res_pin);

    void init();
    void displayOn();
    void displayOff();
    void clearScreen();
    void turnOnAllPixel();
    void refreshScreen();
    void refreshPage(uint8_t page);
    void refreshDirtyArea();
    void setInverseDisplay(bool v);
    void drawCircle8Points(uint8_t x0, uint8_t y0, uint8_t x, uint8_t y, uint8_t color);
    void showCustomChar(uint8_t x, uint8_t y, const uint8_t* charData, uint8_t line=8, uint8_t row=8, uint8_t color=1);
    void showAscii(uint8_t x, uint8_t y, char ch, uint8_t line=8, uint8_t row=8, uint8_t color=1);
    uint8_t reverseBit(uint8_t data);
    void showAsciiStr(uint8_t x, uint8_t y, const char* str, uint8_t Spacing=8, uint8_t line=8, uint8_t row=8, uint8_t color=1);
    void drawPoint(uint8_t x, uint8_t y, uint8_t color);
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color=1);
    void drawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint8_t color=1);
    void drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool fill=false, uint8_t color=1);
    void fillCircle(int x0, int y0, int r, int color);
    void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color=1);

    void setCursor(uint8_t x, uint8_t y);
    void writeCmd(uint8_t cmd);   // 写指令（A0=0）
    void writeData(uint8_t dat);  // 写数据（A0=1）
    void hardwareReset();

    uint8_t getPage(uint8_t y);
    uint8_t getPageOffset(uint8_t y);
        
    uint8_t lcd_buffer[LCD_PAGE][LCD_WIDTH] = {0};
    SPIChannel spi_;  // 硬件SPI通道引用
    GPIO_TypeDef* a0_port_;   // A0引脚（指令/数据）
    uint16_t a0_pin_;
    GPIO_TypeDef* cs_port_;   // CS引脚（片选）
    uint16_t cs_pin_;
    GPIO_TypeDef* res_port_;  // RES引脚（复位）
    uint16_t res_pin_;
private:
    struct DirtyArea {
        uint8_t min_x = LCD_WIDTH, max_x = 0;
        uint8_t min_page = LCD_PAGE, max_page = 0;
    } dirtyArea;

    static const uint8_t ascii8x8[];
};