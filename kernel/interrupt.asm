;结构体定义

;中断管理
;struct Interrupt
InterruptProtect            equ 0     ;保护模式管理模块的引用(dword)
InterruptIdt                equ 4     ;中断描述符符表(dword)
InterruptCount              equ 8     ;中断的总数(中断描述符表中的描述符个数)(dword)
InterruptNestingLevels      equ 12    ;中断嵌套层数(dword)
InterruptIrqHandlerTable    equ 16    ;硬件中断处理函数表的地址(dword)
InterruptSystemCallTable    equ 20    ;系统调用表的地址(dword)
;end struct Interrupt

;栈帧
;struct StackFrame
StackFrameGs           equ 0
StackFrameFs           equ 4
StackFrameEs           equ 8
StackFrameDs           equ 12
StackFrameEdi          equ 16
StackFrameEsi          equ 20
StackFrameEbp          equ 24
StackFrameKernelEsp    equ 28
StackFrameEbx          equ 32
StackFrameEdx          equ 36
StackFrameEcx          equ 40
StackFrameEax          equ 44
StackFrameRetAddr      equ 48
StackFrameEip          equ 52
StackFrameCs           equ 56
StackFrameEflags       equ 60
StackFrameEsp          equ 64
StackFrameSs           equ 68
;end struct StackFrame

;结构体大小
INTERRUPT_SIZE      equ 24
STACK_FRAME_SIZE    equ 72

;8259A端口
INT_M_CTL       equ 0x20
INT_M_CTLMASK   equ 0x21
INT_S_CTL       equ 0xa0
INT_S_CTLMASK   equ 0xa1
;8259A控制字
EOI equ 0x20
;中断向量号
INT_VECTOR_SYSTEM_CALL equ 80h

;外部引用符号
extern interruptInstance            ;中断管理结构体
extern interruptExceptionHandler    ;异常处理程序
extern kernelStack                  ;内核栈
extern restartProcess               ;恢复进程执行
extern saveProcessState             ;保存进程状态

;异常
global divideError
global singleStepException
global nmi
global breakpointException
global overflow
global boundsCheck
global invalOpcode
global coprNotAvailable
global doubleFault
global coprSegOverrun
global invalTss
global segmentNotPresent
global stackException
global generalProtection
global pageFault
global coprError
;硬件中断
global irq00
global irq01
global irq02
global irq03
global irq04
global irq05
global irq06
global irq07
global irq08
global irq09
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
;系统调用中断
global systemCall0
global systemCall1
global systemCall2
global systemCall3
global systemCallInterrupt

[section .text]
;预定义的异常
divideError:
    push 0xffffffff    ;没有错误码
    push 0            ;中断向量号=0
    jmp EXCEPTION
singleStepException:
    push 0xffffffff    ;没有错误码
    push 1            ;中断向量号=1
    jmp EXCEPTION
nmi:
    push 0xffffffff     ;没有错误码
    push 2              ;中断向量号=2
    jmp EXCEPTION
breakpointException:
    push 0xffffffff    ;没有错误码
    push 3            ;中断向量号=3
    jmp EXCEPTION
overflow:
    push 0xffffffff    ;没有错误码
    push 4            ;中断向量号=4
    jmp EXCEPTION
boundsCheck:
    push 0xffffffff    ;没有错误码
    push 5            ;中断向量号=5
    jmp EXCEPTION
invalOpcode:
    push 0xffffffff    ;没有错误码
    push 6            ;中断向量号=6
    jmp EXCEPTION
coprNotAvailable:
    push 0xffffffff    ;没有错误码
    push 7            ;中断向量号=7
    jmp EXCEPTION
doubleFault:
    push 8            ;中断向量号=8
    jmp EXCEPTION
coprSegOverrun:
    push 0xffffffff    ;没有错误码
    push 9            ;中断向量号=9
    jmp EXCEPTION
invalTss:
    push 10            ;中断向量号=A
    jmp EXCEPTION
segmentNotPresent:
    push 11            ;中断向量号=B
    jmp EXCEPTION
stackException:
    push 12            ;中断向量号=C
    jmp EXCEPTION
generalProtection:
    push 13            ;中断向量号=D
    jmp EXCEPTION
pageFault:
    push 14            ;中断向量号=E
    jmp EXCEPTION
coprError:
    push 0xffffffff    ;没有错误码
    push 16            ;中断向量号=10h
    jmp EXCEPTION

EXCEPTION:
    call interruptExceptionHandler
    add esp, 8
    iretd

%macro irqMaster 1
    ;保存进程状态
    call saveProcessState
    ;段寄存器换到内核态
    mov ax, ss
    mov ds, ax
    mov es, ax
    mov fs, ax
    ;判断中断嵌套层数
    mov ebx, [interruptInstance]
    cmp dword [ebx + InterruptNestingLevels], 0
    jne NOT_NESTING_LEVEL_0_irq%1
    mov esp, kernelStack        ;未发生中断重入(说明此时是从ring3->ring0)则切换堆栈，并最后恢复进程执行
    push restartProcess
    jmp END_COMPARE_NESTING_LEVELS_irq%1
NOT_NESTING_LEVEL_0_irq%1:
    push returnParentInterrupt  ;发生了中断重入(说明此时是从ring0->ring0)则不切换堆栈，并最后回到父中断
END_COMPARE_NESTING_LEVELS_irq%1:
    ;不允许再发生当前中断
    in al, INT_M_CTLMASK
    or al, (1 << %1)
    out INT_M_CTLMASK, al
    ;输出EOI
    mov al, EOI
    out INT_M_CTL, al
    ;中断嵌套层数+1
    inc dword [ebx + InterruptNestingLevels]
    sti
    call [ebx + InterruptIrqHandlerTable + %1 * 4]
    cli
    dec dword [ebx + InterruptNestingLevels]
    ;允许当前中断
    in al, INT_M_CTLMASK
    and al, ~(1 << %1)
    out INT_M_CTLMASK, al
    ret
%endmacro

%macro irqSlave 1
    ;保存进程状态
    call saveProcessState
    ;段寄存器、栈换到内核态
    mov ax, ss
    mov ds, ax
    mov es, ax
    mov fs, ax
    ;判断中断嵌套层数
    mov ebx, [interruptInstance]
    cmp dword [ebx + InterruptNestingLevels], 0
    jne NOT_NESTING_LEVEL_0_irq%1
    mov esp, kernelStack        ;未发生中断重入(说明此时是从ring3->ring0)则切换堆栈，并最后恢复进程执行
    push restartProcess
    jmp END_COMPARE_NESTING_LEVELS_irq%1
NOT_NESTING_LEVEL_0_irq%1:
    push returnParentInterrupt  ;发生了中断重入(说明此时是从ring0->ring0)则不切换堆栈，并最后回到父中断
END_COMPARE_NESTING_LEVELS_irq%1:
    ;不允许再发生当前中断
    in al, INT_S_CTLMASK
    or al, (1 << (%1 - 8))
    out INT_S_CTLMASK, al
    ;输出EOI
    mov al, EOI
    out INT_M_CTL, al
    ;中断嵌套层数+1
    inc dword [ebx + InterruptNestingLevels]
    sti
    call [ebx + InterruptIrqHandlerTable + %1 * 4]
    cli
    dec dword [ebx + InterruptNestingLevels]
    ;允许当前中断
    in al, INT_S_CTLMASK
    and al, ~(1 << (%1 - 8))
    out INT_S_CTLMASK, al
    ret
%endmacro

;硬件中断服务程序
;IRQ0:时钟中断，用于进程调度
irq00:
    ;irqMaster 0
    ;保存进程状态
    call saveProcessState
    ;段寄存器换到内核态
    mov ax, ss
    mov ds, ax
    mov es, ax
    mov fs, ax
    ;判断中断嵌套层数
    mov ebx, [interruptInstance]
    cmp dword [ebx + InterruptNestingLevels], 0
    jne NOT_NESTING_LEVEL_0_irq0
    mov esp, kernelStack        ;未发生中断重入(说明此时是从ring3->ring0)则切换堆栈，并最后恢复进程执行
    push restartProcess
    jmp END_COMPARE_NESTING_LEVELS_irq0
NOT_NESTING_LEVEL_0_irq0:
    push returnParentInterrupt  ;发生了中断重入(说明此时是从ring0->ring0)则不切换堆栈，并最后回到父中断
END_COMPARE_NESTING_LEVELS_irq0:
    ;不允许再发生当前中断
    in al, INT_M_CTLMASK
    or al, (1 << 0)
    out INT_M_CTLMASK, al
    ;输出EOI
    mov al, EOI
    out INT_M_CTL, al
    ;中断嵌套层数+1
    inc dword [ebx + InterruptNestingLevels]
    sti
    mov eax, [ebx + InterruptIrqHandlerTable + 0 * 4]
    call [eax]
    cli
    dec dword [ebx + InterruptNestingLevels]
    ;允许当前中断
    in al, INT_M_CTLMASK
    and al, ~(1 << 0)
    out INT_M_CTLMASK, al
    ret
irq01:
    irqMaster 1
irq02:
    irqMaster 2
irq03:
    irqMaster 3
irq04:
    irqMaster 4
irq05:
    irqMaster 5
irq06:
    irqMaster 6
irq07:
    irqMaster 7
irq08:
    irqSlave 8
irq09:
    irqSlave 9
irq10:
    irqSlave 10
irq11:
    irqSlave 11
irq12:
    irqSlave 12
irq13:
    irqSlave 13
irq14:
    irqSlave 14
irq15:
    irqSlave 15

;系统调用中断(int 80h)
systemCallInterrupt:
    ;保存进程状态
    call saveProcessState
    ;段寄存器、栈换到内核态
    mov ebp, esp
    mov esp, kernelStack
    push eax
    mov ax, ss
    mov ds, ax
    mov es, ax
    mov fs, ax
    pop eax
    ;中断嵌套层数+1
    mov ebx, [interruptInstance]
    inc dword [ebx + InterruptNestingLevels]
    sti
    push edx
    push ecx
    push ebx
    mov eax, [ebx + InterruptSystemCallTable + eax * 4]
    call [eax]
    mov [ebp + StackFrameEax], eax
    add esp, 12
    cli
    dec dword [ebx + InterruptNestingLevels]
    ;判断中断嵌套层数
    cmp dword [ebx + InterruptNestingLevels], 0
    je restartProcess
    jmp returnParentInterrupt

;执行0个参数的系统调用
;u32 sysCall0();
systemCall0:
    mov eax, [esp+4]
    int INT_VECTOR_SYSTEM_CALL
    ret
;执行1个参数的系统调用
;u32 sysCall1(u32 arg1);
systemCall1:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    int INT_VECTOR_SYSTEM_CALL
    ret
;执行2个参数的系统调用
;u32 sysCall2(u32 arg1, u32 arg2);
systemCall2:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    mov ecx, [esp+12]
    int INT_VECTOR_SYSTEM_CALL
    ret
;执行3个参数的系统调用
;u32 sysCall3(u32 arg1, u32 arg2, u32 arg3);
systemCall3:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    mov ecx, [esp+12]
    mov edx, [esp+16]
    int INT_VECTOR_SYSTEM_CALL
    ret

returnParentInterrupt:
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
    iretd

