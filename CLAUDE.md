# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

STM32F103C8T6嵌入式项目，**必须使用华为LiteOS操作系统**，通过USART1串口接收命令控制三个LED灯（红灯PA1，绿灯PA2，蓝灯PA3），并实现温度采集功能。

**重要：本项目必须使用LiteOS，不能使用FreeRTOS或其他RTOS替代方案。**

## 硬件配置

- **MCU**: STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB RAM)
- **时钟**: HSI 8MHz (内部高速时钟，未使用PLL倍频)
- **LED引脚**: PA1(红灯), PA2(绿灯) - 推挽输出，低电平点亮
- **串口**: USART1 (PA9-TX, PA10-RX), 115200波特率, 8N1
- **调试接口**: SWD (PA13-SWDIO, PA14-SWCLK)

## 开发工具链

- **IDE**: Keil MDK-ARM V5.32
- **配置工具**: STM32CubeMX 6.11.1
- **HAL库**: STM32Cube FW_F1 V1.8.6
- **项目文件**: `MDK-ARM/project.uvprojx`

## 构建和烧录

### 使用Keil IDE
1. 打开项目: `MDK-ARM/project.uvprojx`
2. 构建: Project → Build Target (F7)
3. 烧录: Flash → Download (F8)
4. 调试: Debug → Start/Stop Debug Session (Ctrl+F5)

### 命令行构建 (如果配置了UV4命令行工具)
```bash
# 构建项目
UV4 -b MDK-ARM/project.uvprojx -o build.log

# 清理
UV4 -c MDK-ARM/project.uvprojx
```

## 串口通信

### 连接参数
- 波特率: 115200
- 数据位: 8
- 停止位: 1
- 校验: 无
- 流控: 无

### 支持的命令
- `RED_LED_ON` - 红灯亮 (PA1输出低电平)
- `RED_LED_OFF` - 红灯灭 (PA1输出高电平)
- `GREEN_LED_ON` - 绿灯亮 (PA2输出低电平)
- `GREEN_LED_OFF` - 绿灯灭 (PA2输出高电平)

命令必须以`\r`或`\n`结尾。

## 代码架构

### 目录结构
```
Core/
├── Src/
│   ├── main.c              # 主程序，包含LED控制逻辑
│   ├── usart.c             # USART1初始化
│   ├── stm32f1xx_it.c      # 中断服务程序
│   ├── stm32f1xx_hal_msp.c # HAL MSP初始化
│   └── system_stm32f1xx.c  # 系统初始化
├── Inc/
│   ├── main.h              # 引脚定义
│   ├── usart.h
│   ├── stm32f1xx_it.h
│   └── stm32f1xx_hal_conf.h # HAL配置
Drivers/
├── STM32F1xx_HAL_Driver/   # ST官方HAL驱动
└── CMSIS/                  # ARM CMSIS库
MDK-ARM/                    # Keil项目文件
project.ioc                 # STM32CubeMX配置文件
```

### 关键代码流程

1. **初始化流程** (main.c):
   - `HAL_Init()` - HAL库初始化
   - `SystemClock_Config()` - 配置系统时钟为HSI 8MHz
   - `MX_GPIO_Init()` - 初始化GPIO (PA1, PA2为推挽输出)
   - `MX_USART1_UART_Init()` - 初始化串口
   - `HAL_UART_Receive_IT()` - 启动串口接收中断

2. **串口接收机制**:
   - 使用中断方式接收，每次接收1字节
   - `HAL_UART_RxCpltCallback()` - 接收完成回调，累积字符到缓冲区
   - 遇到`\r`或`\n`时调用`Process_Command()`处理命令
   - 缓冲区大小: 64字节

3. **命令处理** (`Process_Command()`):
   - 使用`strcmp()`匹配命令字符串
   - 调用`HAL_GPIO_WritePin()`控制LED
   - 通过`HAL_UART_Transmit()`发送响应

### LED控制逻辑
- LED为**共阳极**或**低电平驱动**：`GPIO_PIN_RESET`点亮，`GPIO_PIN_SET`熄灭
- 初始状态：两个LED都熄灭（高电平）

## 修改硬件配置

1. 打开 `project.ioc` 使用STM32CubeMX
2. 修改引脚配置、时钟、外设等
3. 点击 "Generate Code"
4. **重要**: 用户代码必须放在 `/* USER CODE BEGIN */` 和 `/* USER CODE END */` 之间，否则重新生成代码时会被覆盖

## 添加新功能

### 添加新的串口命令
在 `Process_Command()` 函数中添加新的 `else if` 分支：
```c
else if(strcmp((char*)cmd, "NEW_COMMAND") == 0)
{
    // 执行操作
    sprintf(response, "Response message\r\n");
}
```

### 添加新的GPIO外设
1. 在STM32CubeMX中配置新引脚
2. 重新生成代码
3. 在 `main.h` 中会自动生成引脚宏定义
4. 在用户代码区域使用 `HAL_GPIO_WritePin()` 或 `HAL_GPIO_ReadPin()`

### 添加新的外设 (如I2C, SPI)
1. 在STM32CubeMX中启用外设
2. 配置参数（时钟、引脚、模式等）
3. 生成代码后会在 `Core/Src/` 和 `Core/Inc/` 中创建对应的初始化文件
4. 在 `main()` 中会自动添加初始化函数调用

## 调试技巧

### 使用串口调试
```c
char debug_msg[100];
sprintf(debug_msg, "Debug: value=%d\r\n", some_value);
HAL_UART_Transmit(&huart1, (uint8_t*)debug_msg, strlen(debug_msg), 1000);
```

### 使用SWD调试
- 连接ST-Link或J-Link到PA13(SWDIO)和PA14(SWCLK)
- 在Keil中设置断点，使用调试功能查看变量和寄存器

### 常见问题
- **串口无响应**: 检查波特率、TX/RX引脚连接、串口工具设置
- **LED不亮**: 确认引脚配置、检查LED极性（本项目为低电平点亮）
- **命令无法识别**: 确保命令以`\r`或`\n`结尾，检查大小写

## 代码规范

- 用户代码必须放在 `USER CODE BEGIN/END` 标记之间
- 使用HAL库API而非直接操作寄存器（除非性能关键）
- 中断服务程序保持简短，复杂逻辑放在主循环或回调函数
- 串口发送使用阻塞模式 (`HAL_UART_Transmit`)，接收使用中断模式 (`HAL_UART_Receive_IT`)

## Claude Code 工作规则

- **禁止自动创建Markdown文档**: 不要在未经用户明确请求的情况下创建任何`.md`文件（如README.md、文档、总结文件等）。只有当用户明确要求创建文档时才可以创建。
- **简洁明了的回答**: 每次回答要简洁明了，直接切入重点，避免冗长的解释和不必要的细节。
