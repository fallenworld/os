%include "const.inc"


;
;模块外部符号
;
extern kernelStack          ;内核栈
extern restartProcess
extern saveProcessState
extern irqHandlerTable
extern sysCallTable
;屏幕输出函数
extern setPrintMemOffset    ;设置输出位置在显存中的偏移
extern clearScreen          ;清屏
extern printHex             ;输出一个十六进制位
extern printDwordHex        ;输出一个32位数的十六进制形式
extern printWordHex         ;输出一个16位数的十六进制形式
extern printByteHex         ;输出一个8位数的十六进制形式
extern printChar            ;输出一个字符
extern printStr             ;输出一个字符串

;
;模块内部符号
;
extern intNestingLevels     ;中断嵌套层数
extern exceptionHandler
extern hardwareInterruptHandler
;异常
global divide_error
global single_step_exception
global nmi
global breakpoint_exception
global overflow
global bounds_check
global inval_opcode
global copr_not_available
global double_fault
global copr_seg_overrun
global inval_tss
global segment_not_present
global stack_exception
global general_protection
global page_fault
global copr_error
;硬件中断
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15
;系统调用
global systemCall0
global systemCall1
global systemCall2
global systemCall3
global systemCallInterrupt

[section .text]
;预定义的异常
divide_error:
    push 0xffffffff    ;没有错误码
    push 0            ;中断向量号=0
    jmp exception
single_step_exception:
    push 0xffffffff    ;没有错误码
    push 1            ;中断向量号=1
    jmp exception
nmi:
    push 0xffffffff     ;没有错误码
    push 2              ;中断向量号=2
    jmp exception
breakpoint_exception:
    push 0xffffffff    ;没有错误码
    push 3            ;中断向量号=3
    jmp exception
overflow:
    push 0xffffffff    ;没有错误码
    push 4            ;中断向量号=4
    jmp exception
bounds_check:
    push 0xffffffff    ;没有错误码
    push 5            ;中断向量号=5
    jmp exception
inval_opcode:
    push 0xffffffff    ;没有错误码
    push 6            ;中断向量号=6
    jmp exception
copr_not_available:
    push 0xffffffff    ;没有错误码
    push 7            ;中断向量号=7
    jmp exception
double_fault:
    push 8            ;中断向量号=8
    jmp exception
copr_seg_overrun:
    push 0xffffffff    ;没有错误码
    push 9            ;中断向量号=9
    jmp exception
inval_tss:
    push 10            ;中断向量号=A
    jmp exception
segment_not_present:
    push 11            ;中断向量号=B
    jmp exception
stack_exception:
    push 12            ;中断向量号=C
    jmp exception
general_protection:
    push 13            ;中断向量号=D
    jmp exception
page_fault:
    push 14            ;中断向量号=E
    jmp exception
copr_error:
    push 0xffffffff    ;没有错误码
    push 16            ;中断向量号=10h
    jmp exception

exception:
    call exceptionHandler
    add esp, 8
    iretd

;硬件中断
;IRQ0:时钟中断，用于进程调度
hwint00:
    ;保存进程状态
    call saveProcessState
    ;段寄存器、栈换到内核态
    mov ax, SELECTOR_FLAT_RW
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, SELECTOR_VIDEO
    mov gs, ax
    mov esp, kernelStack
    ;不允许再发生当前中断
    in al, INT_M_CTLMASK
    or al, 1
    out INT_M_CTLMASK, al
    ;输出EOI
    mov al, EOI
    out INT_M_CTL, al
    ;中断嵌套层数+1
    inc dword [intNestingLevels]
    sti
    push 0
    call [irqHandlerTable + 0*4]
    add esp, 4
    cli
    dec dword [intNestingLevels]
    ;允许当前中断
    in al, INT_M_CTLMASK
    and al, 0xfe
    out INT_M_CTLMASK, al
    ;判断中断嵌套层数
    cmp dword [intNestingLevels], 0
    je restartProcess
    jmp returnParentInterrupt
hwint01:
    push 1
    jmp hwinthandler
hwint02:
    push 2
    jmp hwinthandler
hwint03:
    push 3
    jmp hwinthandler
hwint04:
    push 4
    jmp hwinthandler
hwint05:
    push 5
    jmp hwinthandler
hwint06:
    push 6
    jmp hwinthandler
hwint07:
    push 7
    jmp hwinthandler
hwint08:
    push 8
    jmp hwinthandler
hwint09:
    push 9
    jmp hwinthandler
hwint10:
    push 10
    jmp hwinthandler
hwint11:
    push 11
    jmp hwinthandler
hwint12:
    push 12
    jmp hwinthandler
hwint13:
    push 13
    jmp hwinthandler
hwint14:
    push 14
    jmp hwinthandler
hwint15:
    push 15
    jmp hwinthandler

returnParentInterrupt:
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
    iretd

;执行0个参数的系统调用
;u32 sysCall0();
systemCall0:
    mov eax, [esp+4]
    int SYSCALL_INT_VECTOR
    ret
;执行1个参数的系统调用
;u32 sysCall1(u32 arg1);
systemCall1:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    int SYSCALL_INT_VECTOR
    ret
;执行2个参数的系统调用
;u32 sysCall2(u32 arg1, u32 arg2);
systemCall2:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    mov ecx, [esp+12]
    int SYSCALL_INT_VECTOR
    ret
;执行3个参数的系统调用
;u32 sysCall3(u32 arg1, u32 arg2, u32 arg3);
systemCall3:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    mov ecx, [esp+12]
    mov edx, [esp+16]
    int SYSCALL_INT_VECTOR
    ret

;系统调用中断(int 80h)
systemCallInterrupt:
    ;保存进程状态
    call saveProcessState
    ;段寄存器、栈换到内核态
    mov ebp, esp
    mov esp, kernelStack
    push eax
    mov ax, SELECTOR_FLAT_RW
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, SELECTOR_VIDEO
    mov gs, ax
    pop eax
    ;中断嵌套层数+1
    inc dword [intNestingLevels]
    sti
    push edx
    push ecx
    push ebx
    call [sysCallTable + eax*4]
    mov [ebp + Process_stackFrame + StackFrame_eax], eax
    add esp, 12
    cli
    dec dword [intNestingLevels]
    ;判断中断嵌套层数
    cmp dword [intNestingLevels], 0
    je restartProcess
    jmp returnParentInterrupt


hwinthandler:
    hlt
