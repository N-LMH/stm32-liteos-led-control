/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序文件 - LiteOS版本
  * @description    : STM32F103C8 + LiteOS 多线程LED控制和温度采集系统
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

/* LiteOS includes */
#include "los_config.h"
#include "los_task.h"
#include "los_sem.h"
#include "los_queue.h"
#include "los_sys.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_PRIO_LED_BLINK     5
#define TASK_PRIO_UART_CMD      4
#define TASK_PRIO_TEMP          6

#define TASK_STACK_SIZE_LED     512
#define TASK_STACK_SIZE_UART    512
#define TASK_STACK_SIZE_TEMP    512
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define RX_BUFFER_SIZE 64
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_index = 0;
uint8_t rx_data;

/* 任务句柄 */
UINT32 g_taskLedBlinkID;
UINT32 g_taskUartCmdID;
UINT32 g_taskTempID;

/* LED模式枚举 */
typedef enum {
  LED_MODE_INIT,          // 初始模式：蓝绿交替闪烁
  LED_MODE_RED_ON,        // 红灯常亮
  LED_MODE_BLUE_FLASH,    // 蓝灯闪烁10次
  LED_MODE_CRAZY_FLASH,   // 三灯交替乱闪
  LED_MODE_ALL_OFF        // 全部熄灭
} LED_Mode_t;

/* 控制标志 */
volatile LED_Mode_t g_ledMode = LED_MODE_INIT;
volatile uint8_t g_ledFlashingCount = 0;
volatile uint8_t g_tempMonitorOn = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void Process_Command(uint8_t *cmd);
VOID *Task_LED_Blink(VOID *arg);
VOID *Task_UART_Cmd(VOID *arg);
VOID *Task_Temp_Monitor(VOID *arg);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  程序入口函数
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* 初始化HAL库，配置Flash接口和SysTick定时器 */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有配置的外设 */
  MX_GPIO_Init();
  MX_ADC1_Init();         // 初始化ADC1
  MX_USART1_UART_Init();  // 初始化USART1

  /* USER CODE BEGIN 2 */
  // 发送欢迎信息
  char *welcome_msg = "The equipment is ready, please send control commands.\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcome_msg, strlen(welcome_msg), 1000);

  // 启动串口接收中断
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);

  /* 初始化LiteOS */
  UINT32 ret = OsMain();
  if (ret != LOS_OK) {
    char *error_msg = "LiteOS init failed!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
    while(1);
  }

  /* 创建LED闪烁任务 */
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

  /* 创建串口命令处理任务 */
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

  /* 创建温度监控任务 */
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

  /* 启动LiteOS调度器 */
  OsStart();

  /* USER CODE END 2 */

  /* 主循环 - 不会执行到这里 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief 系统时钟配置
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 配置RCC振荡器
  * 使用内部高速时钟HSI (8MHz)
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // 不使用PLL倍频
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 配置CPU、AHB和APB总线时钟
  * 系统时钟 = HSI = 8MHz
  * AHB时钟 = 8MHz (不分频)
  * APB1时钟 = 8MHz (不分频)
  * APB2时钟 = 8MHz (不分频)
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
}

/**
  * @brief GPIO初始化函数
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
  GPIOA->BSRR = GPIO_BSRR_BS1 | GPIO_BSRR_BS2;
  GPIOA->ODR |= GPIO_ODR_ODR1 | GPIO_ODR_ODR2;
/* USER CODE END MX_GPIO_Init_1 */

  /* 使能GPIOA时钟 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 配置GPIO引脚 : PA1(红灯) PA2(绿灯) PA3(蓝灯)
   * 模式: 推挽输出
   * 上拉/下拉: 无（输出模式下上下拉无效）
   * 速度: 低速
   */
  GPIO_InitStruct.Pin = RED_LED_Pin|GREEN_LED_Pin|BLUE_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出模式
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // 输出模式下上下拉无效
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 低速模式
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  HAL_GPIO_WritePin(GPIOA, RED_LED_Pin|GREEN_LED_Pin|BLUE_LED_Pin, GPIO_PIN_SET);

/* USER CODE BEGIN MX_GPIO_Init_2 */

  GPIOA->BSRR = GPIO_BSRR_BS1 | GPIO_BSRR_BS2;
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  串口接收完成回调函数
  * @param  huart: UART句柄
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    // 接收到回车或换行符，处理命令
    if(rx_data == '\r' || rx_data == '\n')
    {
      if(rx_index > 0)  // 如果缓冲区有数据
      {
        rx_buffer[rx_index] = '\0';  // 添加字符串结束符
        Process_Command(rx_buffer);  // 处理命令
      }
      // 无论如何都重置索引和清空缓冲区
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
    }
    else if(rx_index < RX_BUFFER_SIZE - 1)
    {
      rx_buffer[rx_index++] = rx_data;  // 存储接收到的字符
    }
    else
    {
      // 缓冲区满了，清空并重新开始
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
      char *error_msg = "Buffer overflow! Command too long.\r\n";
      HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 1000);
    }

    // 继续接收下一个字节
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  }
}

/**
  * @brief  命令处理函数
  * @param  cmd: 接收到的命令字符串
  * @retval None
  */
void Process_Command(uint8_t *cmd)
{
  char response[128];

  // yes - 红灯常亮
  if(strcmp((char*)cmd, "yes") == 0)
  {
    g_ledMode = LED_MODE_RED_ON;
    sprintf(response, "The red light has been lit.\r\n");
  }
  // ledflashingon - 蓝灯每1.5秒闪烁1次，闪烁10次后停止
  else if(strcmp((char*)cmd, "ledflashingon") == 0)
  {
    g_ledMode = LED_MODE_BLUE_FLASH;
    g_ledFlashingCount = 0;
    sprintf(response, "Blue LED flashing started.\r\n");
  }
  // ledflashingoff - 蓝灯停止闪烁
  else if(strcmp((char*)cmd, "ledflashingoff") == 0)
  {
    g_ledMode = LED_MODE_INIT;
    g_ledFlashingCount = 0;
    sprintf(response, "Blue LED flashing stopped.\r\n");
  }
  // gettempron - 获取温度，绿灯长亮
  else if(strcmp((char*)cmd, "gettempron") == 0)
  {
    g_tempMonitorOn = 1;
    sprintf(response, "Temperature monitoring started.\r\n");
  }
  // gettemproff - 停止温度采集，绿灯熄灭
  else if(strcmp((char*)cmd, "gettemproff") == 0)
  {
    g_tempMonitorOn = 0;
    sprintf(response, "Temperature monitoring stopped.\r\n");
  }
  // no - 三个灯交替乱闪
  else if(strcmp((char*)cmd, "no") == 0)
  {
    g_ledMode = LED_MODE_CRAZY_FLASH;
    sprintf(response, "Crazy flash mode activated.\r\n");
  }
  // ledalloff - 所有灯熄灭
  else if(strcmp((char*)cmd, "ledalloff") == 0)
  {
    g_ledMode = LED_MODE_ALL_OFF;
    g_tempMonitorOn = 0;
    sprintf(response, "All LEDs turned off.\r\n");
  }
  else
  {
    sprintf(response, "Unknown command: %s\r\n", (char*)cmd);
  }

  // 发送响应
  HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 1000);
}

/**
  * @brief  LED闪烁任务 - 根据模式控制LED
  * @retval None
  */
VOID *Task_LED_Blink(VOID *arg)
{
  (void)arg;
  uint8_t state = 0;
  static LED_Mode_t lastMode = LED_MODE_INIT;

  while(1)
  {
    // 模式切换时，关闭LED任务控制的LED
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
        // 蓝灯和绿灯交替闪烁，间隔1s
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
        // 红灯常亮（温度监控开启时不控制绿灯）
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
        if (!g_tempMonitorOn) {
          HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        }
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
        LOS_TaskDelay(100);
        break;

      case LED_MODE_BLUE_FLASH:
        // 蓝灯每1.5秒闪烁1次，闪烁10次后停止
        if (g_ledFlashingCount < 10) {
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
          LOS_TaskDelay(100);
          HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
          LOS_TaskDelay(1400);
          g_ledFlashingCount++;
        } else {
          // 闪烁完成，回到初始模式
          g_ledMode = LED_MODE_INIT;
        }
        break;

      case LED_MODE_CRAZY_FLASH:
        // 三个灯交替乱闪（温度监控开启时不控制绿灯）
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
        // 所有灯熄灭
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
  // Task should never return
}

/**
  * @brief  串口命令处理任务
  * @retval None
  */
VOID *Task_UART_Cmd(VOID *arg)
{
  (void)arg;
  while(1)
  {
    // 这个任务主要由串口中断驱动，这里只是保持任务活跃
    LOS_TaskDelay(100);
  }
  // Task should never return
}

/**
  * @brief  温度监控任务
  * @retval None
  */
VOID *Task_Temp_Monitor(VOID *arg)
{
  (void)arg;
  uint32_t adc_value;
  float voltage, temperature;
  char temp_msg[64];
  static uint8_t lastTempState = 0;

  while(1)
  {
    // 温度监控状态变化时控制绿灯
    if (g_tempMonitorOn != lastTempState) {
      if (g_tempMonitorOn) {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET); // 绿灯亮
      } else {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);   // 绿灯灭
      }
      lastTempState = g_tempMonitorOn;
    }

    if (g_tempMonitorOn) {
      // 启动ADC转换
      HAL_ADC_Start(&hadc1);

      // 等待转换完成
      if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        // 读取ADC值
        adc_value = HAL_ADC_GetValue(&hadc1);

        // 计算电压 (12位ADC, 参考电压3.3V)
        voltage = (float)adc_value * 3.3f / 4096.0f;

        // NTC 10K热敏电阻温度计算
        // 使用简化的线性近似公式
        // 实际应用中需要根据具体热敏电阻的B值和分压电路调整
        temperature = (voltage - 0.76f) / 0.0025f + 25.0f;

        // 发送温度值
        sprintf(temp_msg, "Temperature: %.2f C (ADC: %lu, V: %.2f)\r\n",
                temperature, adc_value, voltage);
        HAL_UART_Transmit(&huart1, (uint8_t*)temp_msg, strlen(temp_msg), 1000);
      }

      HAL_ADC_Stop(&hadc1);

      // 每秒发送一次温度
      LOS_TaskDelay(1000);
    } else {
      // 温度监控关闭，延时等待
      LOS_TaskDelay(100);
    }
  }
  // Task should never return
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
