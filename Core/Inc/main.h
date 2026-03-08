/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : 全局项目定义和导出接口
  * @description    : 声明LED引脚映射、错误处理函数和STM32F103 + LiteOS应用使用的共享宏
  *
  * @hardware       : LED引脚分配
  *                   - RED_LED_Pin   = GPIO_PIN_1 (PA1)
  *                   - GREEN_LED_Pin = GPIO_PIN_2 (PA2)
  *                   - BLUE_LED_Pin  = GPIO_PIN_3 (PA3)
  *                   所有LED均为低电平点亮
  *
  * @note           : 此文件由STM32CubeMX生成。用户添加的内容必须保持在
  *                   USER CODE BEGIN/END区域内
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RED_LED_Pin GPIO_PIN_1
#define RED_LED_GPIO_Port GPIOA
#define GREEN_LED_Pin GPIO_PIN_2
#define GREEN_LED_GPIO_Port GPIOA
#define BLUE_LED_Pin GPIO_PIN_3
#define BLUE_LED_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

