/* UART output implementation for LiteOS printf */
#include "los_typedef.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

#define UART_WITH_LOCK      1
#define UART_WITHOUT_LOCK   0

void UartPuts(const char *str, unsigned int len, int isLock)
{
    (void)isLock;  /* Ignore lock parameter for simple implementation */

    if (str == NULL || len == 0) {
        return;
    }

    /* Use HAL UART transmit */
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, 1000);
}
