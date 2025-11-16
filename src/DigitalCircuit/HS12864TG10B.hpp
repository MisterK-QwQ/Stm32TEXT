#pragma once
#include "stm32f1xx_hal.h"

#define LCD_WIDTH  128    // 列数（0-127）
#define LCD_HEIGHT 64    // 行数（0-63）
#define LCD_PAGE   8     // 页数（64行 ÷ 8行/页 = 8页）  // 新增页定义

class HS12864TG10B {
public:
    HS12864TG10B(GPIO_TypeDef* scl_port, uint16_t scl_pin,
                 GPIO_TypeDef* sda_port, uint16_t sda_pin,
                 GPIO_TypeDef* a0_port,  uint16_t a0_pin,
                 GPIO_TypeDef* cs_port,  uint16_t cs_pin,
                 GPIO_TypeDef* res_port, uint16_t res_pin);
    void init();                  // 完整初始化（复位+指令配置）
    void displayOn();             // 开启显示
    void displayOff();            // 关闭显示
    void clearScreen();           // 清屏（填充黑色）
    void turnOnAllPixel();        // 强制全亮（测试用）
    void refreshScreen();
    void setInverseDisplay(bool v);
    void drawCircle8Points(uint8_t x0, uint8_t y0, uint8_t x, uint8_t y, uint8_t color) ;
    void showCustomChar(uint8_t x, uint8_t y, const uint8_t* charData,uint8_t line=8,uint8_t row=8,uint8_t color=1);
    // 显示8x8 ASCII字符（x：列0-127，y：行0-7，color：0=黑，1=白）
    void showAscii(uint8_t x, uint8_t y, char ch,uint8_t line=8,uint8_t row=8, uint8_t color=1);
    uint8_t reverseBit(uint8_t data);
    void showAsciiStr(uint8_t x, uint8_t y, const char* str,uint8_t Spacing=8,uint8_t line=8,uint8_t row=8,uint8_t color=1);
    // 画点（x：0-127，y：0-63，color：0=黑，1=白）
    void drawPoint(uint8_t x, uint8_t y, uint8_t color);
    // 画斜直线（x1,y1：起点；x2,y2：终点；color：颜色）
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color=1);
    // 画三角形（x1,y1/x2,y2/x3,y3：三个顶点；color：颜色）
    void drawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint8_t color=1);

    void drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,bool fill=false, uint8_t color=1);
    void fillCircle(int x0, int y0, int r, int color);
    void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color=1);
private:
    GPIO_TypeDef* scl_port_;  // SPI_SCL引脚（PA5）
    uint16_t scl_pin_;
    GPIO_TypeDef* sda_port_;  // SPI_SDA引脚（PA7）
    uint16_t sda_pin_;
    GPIO_TypeDef* a0_port_;   // A0引脚（指令/数据，PA2）
    uint16_t a0_pin_;
    GPIO_TypeDef* cs_port_;   // CS引脚（片选，PA3）
    uint16_t cs_pin_;
    GPIO_TypeDef* res_port_;  // RES引脚（复位，PA4）
    uint16_t res_pin_;

    void writeBit(uint8_t bit);   // 写1位SPI数据（带时序）
    void writeCmd(uint8_t cmd);   // 写指令（A0=0）
    void writeData(uint8_t dat);  // 写数据（A0=1）
    void hardwareReset();         // 硬件复位（符合规格书时序）
    void setCursor(uint8_t x, uint8_t y);  // 设置光标（x=列，y=页）
    uint8_t getPage(uint8_t y);   // 行→页转换（y=0-63 → 页=0-7）
    uint8_t getPageOffset(uint8_t y);  // 行→页内偏移（y=0-63 → 偏移0-7）

   uint8_t lcd_buffer[LCD_PAGE][LCD_WIDTH] = {0};  // 现在仅1024字节
    static const uint8_t ascii8x8[];                // 8x8 ASCII字库

};