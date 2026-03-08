        PRESERVE8
        THUMB

OS_NVIC_INT_CTRL        EQU     0xE000ED04
OS_NVIC_SYSPRI2         EQU     0xE000ED20
OS_NVIC_PENDSV_PRI      EQU     0xF0F00000
OS_NVIC_PENDSVSET       EQU     0x10000000
OS_TASK_STATUS_RUNNING  EQU     0x0010

        IMPORT  g_oldTask
        IMPORT  g_runTask

        AREA    |.text|, CODE, READONLY

        EXPORT  OsStartToRun
OsStartToRun PROC
        LDR     r4, =OS_NVIC_SYSPRI2
        LDR     r5, =OS_NVIC_PENDSV_PRI
        STR     r5, [r4]

        LDR     r1, =g_oldTask
        STR     r0, [r1]

        LDR     r1, =g_runTask
        STR     r0, [r1]

        MOVS    r1, #2
        MSR     CONTROL, r1
        ISB

        MOVS    r2, #OS_TASK_STATUS_RUNNING
        STRH    r2, [r0, #4]

        LDR     r12, [r0]
        ADDS    r12, r12, #36
        LDMIA   r12!, {r0-r7}
        MSR     PSP, r12

        MOV     lr, r5
        CPSIE   I
        BX      r6
        ENDP

        EXPORT  ArchIntLock
ArchIntLock PROC
        MRS     r0, PRIMASK
        CPSID   I
        BX      lr
        ENDP

        EXPORT  ArchIntUnlock
ArchIntUnlock PROC
        MRS     r0, PRIMASK
        CPSIE   I
        BX      lr
        ENDP

        EXPORT  ArchIntRestore
ArchIntRestore PROC
        MSR     PRIMASK, r0
        BX      lr
        ENDP

        EXPORT  OsTaskSchedule
OsTaskSchedule PROC
        LDR     r2, =OS_NVIC_INT_CTRL
        LDR     r3, =OS_NVIC_PENDSVSET
        STR     r3, [r2]
        BX      lr
        ENDP

        EXPORT  osPendSV
osPendSV PROC
        MRS     r12, PRIMASK
        CPSID   I

        MRS     r0, PSP
        STMDB   r0!, {r4-r12}

        LDR     r5, =g_oldTask
        LDR     r1, [r5]
        STR     r0, [r1]

        LDR     r0, =g_runTask
        LDR     r0, [r0]
        STR     r0, [r5]
        LDR     r1, [r0]

        LDMIA   r1!, {r4-r12}
        MSR     PSP, r1

        MSR     PRIMASK, r12
        BX      lr
        ENDP

        END
