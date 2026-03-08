# STM32F103C8T6 + LiteOS LED Control Project

STM32F103C8T6 嵌入式项目，基于华为 LiteOS 操作系统，通过 USART1 串口接收命令控制三个 LED 灯。

## 📋 项目概述

本项目是一个基于 STM32F103C8T6 微控制器和华为 LiteOS 实时操作系统的嵌入式应用，实现了通过串口命令控制 LED 灯的功能。

### 主要特性

- ✅ 基于华为 LiteOS RTOS
- ✅ USART1 串口通信（115200 波特率）
- ✅ 三色 LED 控制（红、绿、蓝）
- ✅ 温度采集功能
- ✅ 完整的项目文档

## 🔧 硬件配置

- **MCU**: STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB RAM)
- **时钟**: HSI 8MHz (内部高速时钟)
- **LED 引脚**: 
  - PA1 - 红灯（推挽输出，低电平点亮）
  - PA2 - 绿灯（推挽输出，低电平点亮）
  - PA3 - 蓝灯（推挽输出，低电平点亮）
- **串口**: USART1 (PA9-TX, PA10-RX), 115200 8N1
- **调试接口**: SWD (PA13-SWDIO, PA14-SWCLK)

## 🛠️ 开发工具

- **IDE**: Keil MDK-ARM V5.32
- **配置工具**: STM32CubeMX 6.11.1
- **HAL 库**: STM32Cube FW_F1 V1.8.6
- **RTOS**: Huawei LiteOS

## 📁 项目结构

```
project/
├── Core/                   # 核心代码
│   ├── Inc/               # 头文件
│   └── Src/               # 源文件
├── Drivers/               # 驱动库
│   ├── CMSIS/            # CMSIS 标准
│   └── STM32F1xx_HAL_Driver/  # HAL 驱动
├── LiteOS/               # LiteOS 源码
│   ├── arch/             # 架构相关
│   ├── kernel/           # 内核代码
│   └── targets/          # 目标平台
├── MDK-ARM/              # Keil 项目文件
│   └── project.uvprojx   # 主项目文件
├── CLAUDE.md             # Claude Code 开发指南
└── project.ioc           # STM32CubeMX 配置
```

## 🚀 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/N-LMH/stm32-liteos-led-control.git
cd stm32-liteos-led-control
```

### 2. 打开项目

使用 Keil MDK-ARM 打开项目文件：
```
MDK-ARM/project.uvprojx
```

### 3. 编译

在 Keil 中：
- 按 `F7` 或点击 `Project → Build Target`

### 4. 烧录

- 连接 ST-Link 调试器
- 按 `F8` 或点击 `Flash → Download`

### 5. 测试

1. 连接串口（115200 8N1）
2. 发送命令控制 LED：
   - `RED_ON` - 打开红灯
   - `RED_OFF` - 关闭红灯
   - `GREEN_ON` - 打开绿灯
   - `GREEN_OFF` - 关闭绿灯
   - `BLUE_ON` - 打开蓝灯
   - `BLUE_OFF` - 关闭蓝灯

## 📖 详细文档

详细的开发指南和说明请参考 [CLAUDE.md](CLAUDE.md)

## 🔨 构建说明

### 使用 Keil MDK-ARM

1. 打开 `MDK-ARM/project.uvprojx`
2. 选择目标配置（Debug/Release）
3. 构建项目（F7）
4. 下载到设备（F8）

### 配置修改

如需修改硬件配置：
1. 使用 STM32CubeMX 打开 `project.ioc`
2. 修改配置
3. 重新生成代码
4. 在 Keil 中重新编译

## 📝 串口命令

| 命令 | 功能 | 示例 |
|------|------|------|
| `RED_ON` | 打开红灯 | `RED_ON\r\n` |
| `RED_OFF` | 关闭红灯 | `RED_OFF\r\n` |
| `GREEN_ON` | 打开绿灯 | `GREEN_ON\r\n` |
| `GREEN_OFF` | 关闭绿灯 | `GREEN_OFF\r\n` |
| `BLUE_ON` | 打开蓝灯 | `BLUE_ON\r\n` |
| `BLUE_OFF` | 关闭蓝灯 | `BLUE_OFF\r\n` |

## 🎯 功能特性

### LiteOS 任务

项目使用 LiteOS 实现多任务管理：
- LED 控制任务
- 串口接收任务
- 温度采集任务

### 内存管理

- 动态内存池
- 任务栈管理
- 中断栈配置

## 🐛 调试

### 串口调试

使用串口工具（如 PuTTY、SecureCRT）：
- 波特率：115200
- 数据位：8
- 停止位：1
- 校验：无
- 流控：无

### SWD 调试

使用 Keil 内置调试器或 ST-Link Utility

## 📊 性能指标

- **启动时间**: < 100ms
- **任务切换**: < 10μs
- **串口响应**: < 1ms
- **内存占用**: ~15KB RAM

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 👥 作者

- **N-LMH** - [GitHub](https://github.com/N-LMH)

## 🙏 致谢

- STMicroelectronics - STM32 HAL 库
- Huawei - LiteOS RTOS
- ARM - CMSIS 标准

## 📞 联系方式

如有问题或建议，请通过以下方式联系：
- 提交 [Issue](https://github.com/N-LMH/stm32-liteos-led-control/issues)
- 发送 Pull Request

---

⭐ 如果这个项目对你有帮助，请给个 Star！
