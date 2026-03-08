/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Target Config HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-07-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include "los_typedef.h"
#include "stm32f1xx.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*=============================================================================
                                        System clock module configuration
=============================================================================*/
#define OS_SYS_CLOCK                                        8000000  // 8MHz HSI
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    1000     // 1ms tick
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  0xFFFFFFUL

/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_HWI                                 1
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 0
#define LOSCFG_PLATFORM_HWI_LIMIT                           96
#define LOSCFG_HWI_PRIO_LIMIT                               16
#define OS_USER_HWI_MIN                                     0
#define OS_USER_HWI_MAX                                     96

/*=============================================================================
                                       Task module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_TSK_LIMIT                          15
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (0x500U)
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (0x500U)
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (0x200U)
#define LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE               (0x500U)
#define LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO                   10
#define LOSCFG_BASE_CORE_TIMESLICE                          1
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  10
#define LOSCFG_BASE_CORE_TSK_MONITOR                        0
#define LOSCFG_TASK_MIN_STACK_SIZE                          LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE
#define LOSCFG_BASE_CORE_USE_MULTI_LIST                     1

/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_SEM                                 1
#define LOSCFG_BASE_IPC_SEM_LIMIT                           20

/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_MUX                                 1
#define LOSCFG_BASE_IPC_MUX_LIMIT                           20

/*=============================================================================
                                       Queue module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_QUEUE                               1
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         10

/*=============================================================================
                                       Event module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_EVENT                               1

/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_SWTMR                              1
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        0
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        16
#define LOSCFG_BASE_CORE_SWTMR_IN_HEAD                      1

/*=============================================================================
                                       Memory module configuration
=============================================================================*/
#define LOSCFG_MEMORY_BESTFIT                               1
/* Note: LOSCFG_MEM_WATERLINE is auto-defined in los_config.h based on LOSCFG_DEBUG_VERSION */
#define LOSCFG_SYS_HEAP_SIZE                                (12 * 1024)
extern UINT8 g_liteOsHeap[LOSCFG_SYS_HEAP_SIZE];
#define OS_SYS_MEM_ADDR                                     ((VOID *)&g_liteOsHeap[0])
#define OS_SYS_MEM_SIZE                                     LOSCFG_SYS_HEAP_SIZE

/*=============================================================================
                                       fw Interface configuration
=============================================================================*/
#define LOSCFG_COMPAT_CMSIS_FW                              0

/*=============================================================================
                                       Dynamic loading configuration
=============================================================================*/
#define LOS_DL_HEAP_BASE                                    0
#define LOS_DL_HEAP_SIZE                                    0

/*=============================================================================
                                       others
=============================================================================*/
#define LOSCFG_BASE_CORE_SCHED_SLEEP                        1
#define LOSCFG_PLATFORM_OSAPPINIT                           1

/*=============================================================================
                                       Kernel feature configuration
=============================================================================*/
/* Note: Features are disabled by NOT defining them (not by defining as 0) */
/* Uncomment to enable features:
 * #define LOSCFG_KERNEL_PRINTF
 * #define LOSCFG_KERNEL_CPUP
 * #define LOSCFG_KERNEL_RUNSTOP
 * #define LOSCFG_KERNEL_TICKLESS
 * #define LOSCFG_KERNEL_TRACE
 * #define LOSCFG_KERNEL_LOWPOWER
 * #define LOSCFG_KERNEL_DYNLOAD
 * #define LOSCFG_KERNEL_SCATTER
 * #define LOSCFG_KERNEL_SMP
 * #define LOSCFG_KERNEL_PERF
 */

/*=============================================================================
                                       Debug configuration
=============================================================================*/
/* Note: Debug features are disabled by NOT defining them */
/* Uncomment to enable debug features:
 * #define LOSCFG_DEBUG_VERSION
 * #define LOSCFG_DEBUG_QUEUE
 * #define LOSCFG_DEBUG_SEMAPHORE
 * #define LOSCFG_DEBUG_MUTEX
 * #define LOSCFG_MEM_LEAKCHECK
 */
/* LOSCFG_MEM_WATERLINE is auto-defined in los_config.h when LOSCFG_DEBUG_VERSION is set */

/* Platform adaptation interfaces */
extern VOID HalClockInit(VOID);
extern VOID HalClockStart(VOID);
extern UINT64 HalClockGetCycles(VOID);
extern VOID HalDelayUs(UINT32 usecs);
extern VOID HalIrqInit(VOID);
#ifdef LOSCFG_PLATFORM_OSAPPINIT
extern UINT32 osAppInit(VOID);
extern VOID app_init(VOID);
#endif

/* Linker symbols */
extern CHAR __bss_end;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
