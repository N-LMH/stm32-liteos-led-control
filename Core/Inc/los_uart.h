#ifndef _LOS_UART_H
#define _LOS_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_WITH_LOCK      1
#define UART_WITHOUT_LOCK   0

void UartPuts(const char *str, unsigned int len, int isLock);

#ifdef __cplusplus
}
#endif

#endif /* _LOS_UART_H */
