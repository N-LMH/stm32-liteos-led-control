/* ----------------------------------------------------------------------------
 * LiteOS adapter for STM32F103
 * --------------------------------------------------------------------------- */

#include "los_config.h"
#include "los_memory.h"
#include "stm32f1xx_hal.h"

/* Heap region exposed to LiteOS through OS_SYS_MEM_ADDR */
UINT8 g_liteOsHeap[LOSCFG_SYS_HEAP_SIZE];
extern UINTPTR g_sys_mem_addr_end;

/* 64-bit cycle extension based on DWT CYCCNT */
static UINT32 g_lastCycle = 0;
static UINT64 g_cycleHigh = 0;

VOID HalClockInit(VOID)
{
    g_sys_mem_addr_end = (UINTPTR)(g_liteOsHeap + sizeof(g_liteOsHeap));

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    g_lastCycle = 0;
    g_cycleHigh = 0;
}

VOID HalClockStart(VOID)
{
    /* SysTick is configured by HAL_Init/SystemClock_Config. */
}

UINT64 HalClockGetCycles(VOID)
{
    UINT32 cycle = DWT->CYCCNT;

    if (cycle < g_lastCycle) {
        g_cycleHigh += (1ULL << 32);
    }
    g_lastCycle = cycle;

    return g_cycleHigh | cycle;
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT32 start = DWT->CYCCNT;
    UINT32 waitCycles = (UINT32)(((UINT64)SystemCoreClock * usecs) / 1000000ULL);

    while ((UINT32)(DWT->CYCCNT - start) < waitCycles) {
    }
}

VOID ArchExcInit(VOID)
{
}

VOID ArchBackTrace(VOID)
{
}

VOID ArchBackTraceWithSp(const VOID *stackPointer)
{
    (VOID)stackPointer;
}

VOID OsBackTrace(VOID)
{
}
