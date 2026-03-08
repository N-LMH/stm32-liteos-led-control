/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Secure C Library Stub
 * Author: Huawei LiteOS Team
 * Create: 2021-07-20
 * --------------------------------------------------------------------------- */

#ifndef _SECUREC_H
#define _SECUREC_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Secure string functions - simplified implementation */
#define memcpy_s(dest, destMax, src, count) \
    ((count) <= (destMax) ? (memcpy((dest), (src), (count)), 0) : -1)

#define memset_s(dest, destMax, c, count) \
    ((count) <= (destMax) ? (memset((dest), (c), (count)), 0) : -1)

#define strcpy_s(dest, destMax, src) \
    (strlen(src) < (destMax) ? (strcpy((dest), (src)), 0) : -1)

#define strncpy_s(dest, destMax, src, count) \
    ((count) < (destMax) ? (strncpy((dest), (src), (count)), (dest)[(count)] = '\0', 0) : -1)

#define strcat_s(dest, destMax, src) \
    ((strlen(dest) + strlen(src)) < (destMax) ? (strcat((dest), (src)), 0) : -1)

#define sprintf_s snprintf
#define snprintf_s snprintf
#define vsprintf_s vsnprintf
#define vsnprintf_s(strDest, destMax, count, format, argList) \
    vsnprintf((strDest), (count), (format), (argList))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _SECUREC_H */
