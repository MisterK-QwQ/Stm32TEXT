#pragma once
#include "stm32f1xx_hal.h"
#include "Utils/Logger.hpp"
struct DATA{
  UART_HandleTypeDef huart1;
  IWDG_HandleTypeDef hiwdg;
}Data;

Logger LogF(&Data.huart1);

bool USART1_UART_Init(void) {
  __HAL_RCC_USART1_CLK_ENABLE(); // 提前使能USART1时钟

  Data.huart1.Instance = USART1;
  Data.huart1.Init.BaudRate = 9600;  // 设置波特率
  Data.huart1.Init.WordLength = UART_WORDLENGTH_8B;
  Data.huart1.Init.StopBits = UART_STOPBITS_1;
  Data.huart1.Init.Parity = UART_PARITY_NONE;
  Data.huart1.Init.Mode = UART_MODE_TX_RX;
  Data.huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  Data.huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&Data.huart1) != HAL_OK) {
    return false;
  }
  return true;
}

bool IWDG_Init(){
  Data.hiwdg.Instance = IWDG;  // 指定看门狗实例（独立看门狗只有IWDG）
  Data.hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  Data.hiwdg.Init.Reload = 2343;  //Reload + 1 = (15 × 40000) ÷ 256 = 600000 ÷ 256 = 2343.75
  
  if (HAL_IWDG_Init(&Data.hiwdg) != HAL_OK) {
    LogF.logF(LogLevel::ERROR,"IWDG Initialization Failed");
    return false;
  }
  return true;
}

bool SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // 校准HSI
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // 不使用PLL，避免配置复杂
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    return false;
  }

   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; // 系统时钟=HSI（8MHz）
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB=8MHz（SysTick时钟源）
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // APB1=8MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2=8MHz（GPIOC挂在APB2）
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    return false;
  }
  return true;
}


//  EXTI0中断服务函数（



