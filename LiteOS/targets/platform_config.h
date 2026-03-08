/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Platform Config HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-07-20
 * --------------------------------------------------------------------------- */

#ifndef _PLATFORM_CONFIG_H
#define _PLATFORM_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Platform specific configurations */
#define LOSCFG_PLATFORM_STM32F103                           1
#define LOSCFG_KERNEL_CORE_NUM                              1

/* Sortlink configuration */
#define LOSCFG_BASE_CORE_SWTMR_SORTLINK                     1
#define LOSCFG_SORTLINK_LIST                                1

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _PLATFORM_CONFIG_H */
