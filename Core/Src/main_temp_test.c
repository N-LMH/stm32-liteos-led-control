/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main_temp_test.c
  * @brief          : 热敏传感器测试程序（简化版）
  * @description    : 用于单独测试ADC和温度传感器功能
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
  * @brief  程序入口函数 - 温度传感器测试版
  * @retval int
  */
int main(void)
{
  /* 初始化HAL库 */
  HAL_Init();

  /* 配置系统时钟 */
  SystemClock_Config();

  /* 初始化外设 */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();

  /* 发送测试开始信息 */
  char *start_msg = "\r\n=== Temperature Sensor Test Started ===\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)start_msg, strlen(start_msg), 1000);

  char *info_msg = "Reading temperature every 1 second...\r\n\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)info_msg, strlen(info_msg), 1000);

  /* 点亮绿灯表示测试运行中 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

  /* 变量定义 */
  uint32_t adc_value;
  float voltage, temperature;
  char msg[128];
  uint32_t count = 0;

  /* 主循环 */
  while (1)
  {
    count++;

    /* 启动ADC转换 */
    HAL_ADC_Start(&hadc1);

    /* 等待转换完成 */
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
      /* 读取ADC值 */
      adc_value = HAL_ADC_GetValue(&hadc1);

      /* 计算电压 (12位ADC, 参考电压3.3V) */
      voltage = (float)adc_value * 3.3f / 4096.0f;

      /* 温度计算 - 方法1：简化线性公式 */
      temperature = (voltage - 0.76f) / 0.0025f + 25.0f;

      /* 发送详细信息 */
      sprintf(msg, "[%lu] ADC Raw: %4lu | Voltage: %.3fV | Temp: %.2f°C\r\n",
              count, adc_value, voltage, temperature);
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);

      /* 额外的调试信息 */
      if (count % 5 == 0) {
        sprintf(msg, "     (ADC Range: 0-4095, Voltage Range: 0-3.3V)\r\n\r\n");
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
      }
    }
    else
    {
      /* ADC转换失败 */
      char *error_msg = "ERROR: ADC conversion timeout!\r\n";
      HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
    }

    HAL_ADC_Stop(&hadc1);

    /* 绿灯闪烁表示正在工作 */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);

    /* 延时1秒 */
    HAL_Delay(1000);
  }
}

/**
  * @brief 系统时钟配置
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 配置RCC振荡器 - 使用内部高速时钟HSI (8MHz) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 配置CPU、AHB和APB总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO初始化函数
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使能GPIOA时钟 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 配置GPIO引脚 : PA1(红灯) PA2(绿灯) PA3(蓝灯) */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* 初始状态：所有LED熄灭（高电平） */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    /* 错误时红灯快闪 */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    HAL_Delay(100);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
