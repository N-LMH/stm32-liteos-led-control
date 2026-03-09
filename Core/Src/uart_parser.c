// 优化后的串口命令解析器
#include "main.h"
#include <string.h>

// 命令缓冲区
static char cmd_buffer[64];
static uint8_t cmd_index = 0;

// 命令处理函数
void parse_uart_command(char* cmd) {
    if (strcmp(cmd, "RED_ON") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    } else if (strcmp(cmd, "RED_OFF") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    } else if (strcmp(cmd, "GREEN_ON") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    } else if (strcmp(cmd, "GREEN_OFF") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    } else if (strcmp(cmd, "BLUE_ON") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    } else if (strcmp(cmd, "BLUE_OFF") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    }
}

// 接收字符处理
void uart_receive_char(char c) {
    if (c == '\n' || c == '\r') {
        if (cmd_index > 0) {
            cmd_buffer[cmd_index] = '\0';
            parse_uart_command(cmd_buffer);
            cmd_index = 0;
        }
    } else if (cmd_index < sizeof(cmd_buffer) - 1) {
        cmd_buffer[cmd_index++] = c;
    }
}
