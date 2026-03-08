/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32F103C8T6 + LiteOS LED和温度演示程序
  * @description    : 基于华为LiteOS实现UART命令解析、LED模式控制和NTC温度监测
  *
  * @hardware       : MCU  : STM32F103C8T6 (Cortex-M3, 8 MHz HSI)
  *                   LED  : PA1红灯, PA2绿灯, PA3蓝灯, 低电平点亮
  *                   UART : USART1在PA9/PA10，波特率115200
  *                   ADC  : ADC1_CH0在PA0，用于NTC传感器
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

#include "los_config.h"
#include "los_sys.h"
#include "los_task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* LiteOS任务优先级：数值越小优先级越高 */
#define TASK_PRIO_UART_CMD      4
#define TASK_PRIO_LED_BLINK     5
#define TASK_PRIO_TEMP          6

/* LiteOS任务栈大小（字节） */
#define TASK_STACK_SIZE_LED     512
#define TASK_STACK_SIZE_UART    512
#define TASK_STACK_SIZE_TEMP    2048
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* UART接收缓冲区状态 */
#define RX_BUFFER_SIZE 64
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_index = 0;
uint8_t rx_data;

/* LiteOS任务ID */
UINT32 g_taskLedBlinkID;
UINT32 g_taskUartCmdID;
UINT32 g_taskTempID;

/* LED工作模式 */
typedef enum {
  LED_MODE_INIT,
  LED_MODE_RED_ON,
  LED_MODE_BLUE_FLASH,
  LED_MODE_CRAZY_FLASH,
  LED_MODE_ALL_OFF
} LED_Mode_t;

/* 任务间共享状态 */
volatile LED_Mode_t g_ledMode = LED_MODE_INIT;
volatile uint8_t g_ledFlashingCount = 0;
volatile uint8_t g_tempMonitorOn = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
/* 命令处理辅助函数 */
void Process_Command(uint8_t *cmd);

/* 滤波ADC采样辅助函数 */
uint32_t ADC_Read_Average(uint8_t samples);

/* LiteOS任务入口点 */
VOID *Task_LED_Blink(VOID *arg);
VOID *Task_UART_Cmd(VOID *arg);
VOID *Task_Temp_Monitor(VOID *arg);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  多次读取ADC并返回平均值
  * @note   用于降低NTC温度输入的噪声
  * @param  samples: 要平均的采样次数
  * @retval 平均后的12位ADC值
  */
uint32_t ADC_Read_Average(uint8_t samples)
{
  uint32_t sum = 0;

  for(uint8_t i = 0; i < samples; i++)
  {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
      sum += HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    LOS_TaskDelay(10);
  }

  return sum / samples;
}

/* USER CODE END 0 */

/**
  * @brief  应用程序入口点
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU配置--------------------------------------------------------*/

  /* 复位所有外设，初始化Flash接口和Systick */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有配置的外设 */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  char *welcome_msg = "The equipment is ready, please send control commands.\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcome_msg, strlen(welcome_msg), 1000);
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  UINT32 ret = OsMain();
  if (ret != LOS_OK) {
    char *error_msg = "LiteOS init failed!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
    while(1);
  }
  TSK_INIT_PARAM_S taskInitParam;
  memset(&taskInitParam, 0, sizeof(TSK_INIT_PARAM_S));
  taskInitParam.pfnTaskEntry = Task_LED_Blink;
  taskInitParam.uwStackSize = TASK_STACK_SIZE_LED;
  taskInitParam.pcName = "LED_Blink";
  taskInitParam.usTaskPrio = TASK_PRIO_LED_BLINK;
  ret = LOS_TaskCreate(&g_taskLedBlinkID, &taskInitParam);
  if (ret != LOS_OK) {
    char *error_msg = "LED task create failed!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
  }
  memset(&taskInitParam, 0, sizeof(TSK_INIT_PARAM_S));
  taskInitParam.pfnTaskEntry = Task_UART_Cmd;
  taskInitParam.uwStackSize = TASK_STACK_SIZE_UART;
  taskInitParam.pcName = "UART_Cmd";
  taskInitParam.usTaskPrio = TASK_PRIO_UART_CMD;
  ret = LOS_TaskCreate(&g_taskUartCmdID, &taskInitParam);
  if (ret != LOS_OK) {
    char *error_msg = "UART task create failed!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
  }
  memset(&taskInitParam, 0, sizeof(TSK_INIT_PARAM_S));
  taskInitParam.pfnTaskEntry = Task_Temp_Monitor;
  taskInitParam.uwStackSize = TASK_STACK_SIZE_TEMP;
  taskInitParam.pcName = "Temp_Monitor";
  taskInitParam.usTaskPrio = TASK_PRIO_TEMP;
  ret = LOS_TaskCreate(&g_taskTempID, &taskInitParam);
  if (ret != LOS_OK) {
    char *error_msg = "Temp task create failed!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
  }
  OsStart();

  /* USER CODE END 2 */

  /* 无限循环 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief  系统时钟配置
  * @retval 无
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** 根据RCC_OscInitTypeDef结构体中指定的参数初始化RCC振荡器
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 初始化CPU、AHB和APB总线时钟
  */
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO初始化函数
  * @param 无
  * @retval 无
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
  GPIOA->BSRR = GPIO_BSRR_BS1 | GPIO_BSRR_BS2;
  GPIOA->ODR |= GPIO_ODR_ODR1 | GPIO_ODR_ODR2;
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO端口时钟使能 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RED_LED_Pin|GREEN_LED_Pin|BLUE_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : RED_LED_Pin GREEN_LED_Pin BLUE_LED_Pin */
  GPIO_InitStruct.Pin = RED_LED_Pin|GREEN_LED_Pin|BLUE_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  GPIOA->BSRR = GPIO_BSRR_BS1 | GPIO_BSRR_BS2;
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  UART接收完成回调函数
  * @note   将字节收集到行缓冲区，当接收到行结束符时分发命令
  * @param  huart: UART句柄指针
  * @retval 无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    if(rx_data == '\r' || rx_data == '\n')
    {
      if(rx_index > 0)
      {
        rx_buffer[rx_index] = '\0';
        Process_Command(rx_buffer);
      }
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
    }
    else if(rx_index < RX_BUFFER_SIZE - 1)
    {
      rx_buffer[rx_index++] = rx_data;
    }
    else
    {
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
      {
        char *error_msg = "Buffer overflow! Command too long.\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)error_msg, strlen(error_msg), 1000);
      }
    }

    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  }
}

/**
  * @brief  解析并执行UART命令
  * @param  cmd: 以null结尾的命令字符串
  * @retval 无
  *
  * 支持的命令：
  *   - "yes"
  *   - "no"
  *   - "ledflashingon"
  *   - "ledflashingoff"
  *   - "gettempron"
  *   - "gettemproff"
  *   - "ledalloff"
  */
void Process_Command(uint8_t *cmd)
{
  char *response = NULL;

  if(strcmp((char*)cmd, "yes") == 0)
  {
    g_ledMode = LED_MODE_RED_ON;
    response = "The red light has been lit.\r\n";
  }
  else if(strcmp((char*)cmd, "ledflashingon") == 0)
  {
    g_ledMode = LED_MODE_BLUE_FLASH;
    g_ledFlashingCount = 0;
    response = "Blue LED flashing started.\r\n";
  }
  else if(strcmp((char*)cmd, "ledflashingoff") == 0)
  {
    g_ledMode = LED_MODE_INIT;
    g_ledFlashingCount = 0;
    response = "Blue LED flashing stopped.\r\n";
  }
  else if(strcmp((char*)cmd, "gettempron") == 0)
  {
    g_tempMonitorOn = 1;
    response = "Temperature monitoring started.\r\n";
  }
  else if(strcmp((char*)cmd, "gettemproff") == 0)
  {
    g_tempMonitorOn = 0;
    response = "Temperature monitoring stopped.\r\n";
  }
  else if(strcmp((char*)cmd, "no") == 0)
  {
    g_ledMode = LED_MODE_CRAZY_FLASH;
    response = "Crazy flash mode activated.\r\n";
  }
  else if(strcmp((char*)cmd, "ledalloff") == 0)
  {
    g_ledMode = LED_MODE_ALL_OFF;
    g_tempMonitorOn = 0;
    response = "All LEDs turned off.\r\n";
  }
  else
  {
    static char unknown_cmd[96];
    sprintf(unknown_cmd, "Unknown command: %s\r\n", (char *)cmd);
    response = unknown_cmd;
  }

  if (response != NULL)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)response, strlen(response), 1000);
  }
}

/**
  * @brief  LED控制任务
  * @note   根据g_ledMode驱动LED模式
  * @param  arg: 未使用的任务参数
  * @retval 无。此任务永不返回
  */
VOID *Task_LED_Blink(VOID *arg)
{
  (void)arg;
  uint8_t state = 0;
  static LED_Mode_t lastMode = LED_MODE_INIT;

  while(1)
  {
    /* 模式改变时重置LED输出 */
    if (lastMode != g_ledMode) {
      HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
      if (!g_tempMonitorOn) {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
      }
      HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
      state = 0;
      lastMode = g_ledMode;
    }

    switch(g_ledMode)
    {
      case LED_MODE_INIT:
        if (state == 0) {
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
          if (!g_tempMonitorOn) {
            HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
          }
          state = 1;
        } else {
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
          if (!g_tempMonitorOn) {
            HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
          }
          state = 0;
        }
        LOS_TaskDelay(1000);
        break;

      case LED_MODE_RED_ON:
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
        if (!g_tempMonitorOn) {
          HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        }
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
        LOS_TaskDelay(100);
        break;

      case LED_MODE_BLUE_FLASH:
        if (g_ledFlashingCount < 10) {
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
          LOS_TaskDelay(100);
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
          LOS_TaskDelay(1400);
          g_ledFlashingCount++;
        } else {
          g_ledMode = LED_MODE_INIT;
        }
        break;

      case LED_MODE_CRAZY_FLASH:
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
        LOS_TaskDelay(100);
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
        if (!g_tempMonitorOn) {
          HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
        }
        LOS_TaskDelay(100);
        if (!g_tempMonitorOn) {
          HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        }
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
        LOS_TaskDelay(100);
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
        break;

      case LED_MODE_ALL_OFF:
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
        LOS_TaskDelay(100);
        break;

      default:
        LOS_TaskDelay(100);
        break;
    }
  }
}

/**
  * @brief  UART命令任务
  * @note   命令当前直接在UART RX回调中处理
  * @param  arg: 未使用的任务参数
  * @retval 无。此任务永不返回
  */
VOID *Task_UART_Cmd(VOID *arg)
{
  (void)arg;
  while(1)
  {
    LOS_TaskDelay(100);
  }
}

/**
  * @brief  温度监测任务
  * @note   定期采样NTC输入并报告温度
  * @param  arg: 未使用的任务参数
  * @retval 无。此任务永不返回
  */
VOID *Task_Temp_Monitor(VOID *arg)
{
  (void)arg;
  uint32_t adc_value;
  float voltage, temperature;
  char temp_msg[80];
  static uint8_t lastTempState = 0;

  while(1)
  {
    /* 监测状态改变时更新绿色LED */
    if (g_tempMonitorOn != lastTempState) {
      if (g_tempMonitorOn) {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
      } else {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
      }
      lastTempState = g_tempMonitorOn;
    }

    if (g_tempMonitorOn) {
      adc_value = ADC_Read_Average(10);

      /* 将ADC值转换为电压 */
      voltage = (float)adc_value * 3.3f / 4096.0f;

      /* 基于测量校准数据的线性温度近似 */
      temperature = 25.0f - (adc_value - 2120.0f) * 0.0167f;

      /* 通过UART发送温度报告 */
      sprintf(temp_msg, "Temperature: %.2f C (ADC: %lu, V: %.2f)\r\n",
              temperature, adc_value, voltage);
      HAL_UART_Transmit(&huart1, (uint8_t*)temp_msg, strlen(temp_msg), 1000);

      LOS_TaskDelay(900);
    } else {
      LOS_TaskDelay(100);
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  发生错误时执行此函数
  * @retval 无
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 用户可以添加自己的实现来报告HAL错误返回状态 */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  报告发生assert_param错误的源文件名和行号
  * @param  file: 指向源文件名的指针
  * @param  line: assert_param错误行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 用户可以添加自己的实现来报告文件名和行号，
     例如: printf("参数值错误: 文件 %s 第 %d 行\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */




