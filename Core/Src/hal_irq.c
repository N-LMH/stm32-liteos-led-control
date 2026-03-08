/* ----------------------------------------------------------------------------
 * Hardware IRQ Initialization
 * --------------------------------------------------------------------------- */

#include "los_config.h"
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Hardware IRQ initialization function */
VOID HalIrqInit(VOID)
{
    /* STM32 HAL already initializes NVIC, nothing to do here */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
