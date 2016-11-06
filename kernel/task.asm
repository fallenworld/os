;结构体定义

;保护模式
;struct Protect
ProtectGdt              equ 0     ;全局描述符表(dword)
ProtectTss              equ 4     ;任务状态段指针(dword)
ProtectGdtLen           equ 8     ;全局描述符表的长度(dword)
ProtectGdtEnd           equ 12    ;gdt中末尾位置的下一位置的下表(dword)
ProtectSelectorCode     equ 16    ;代码段选择符(word)
ProtectSelectorData     equ 18    ;数据段选择符(word)
ProtectSelectorVideo    equ 20    ;显存段选择符(word)
ProtectSelectorTss      equ 22    ;任务状态段选择符(word)
;end struct Protect

;任务状态段
;struct Tss
TssLastLink     equ 0
TssEsp0         equ 4
;end Struct Tss

;进程栈帧
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

;进程
;struct Task
TaskStackFrame      equ 0       ;保存寄存器状态的栈帧(72bytes)
TaskLdtSelector     equ 72      ;局部描述符表的选择符(word)
TaskLdt             equ 74      ;局部描述符表(16 bytes)
TaskEntry           equ 90      ;进程的执行入口地址(dword)
TaskPid             equ 94      ;进程id(dword)
TaskName            equ 98      ;进程名(16 bytes)
;end struct Task

;进程管理模块
;struct TaskManager
TaskManagerProtect          equ 0       ;保护模式模块的引用
TaskManagerRunning          equ 4       ;当前正在运行的进程(dword)
TaskManagerCount            equ 8       ;系统中的进程数目(dword)
TaskManagerTaskTable        equ 12      ;进程表(dword)
TaskManagerTaskTableLen     equ 16      ;进程表的最大长度(dword)
TaskManagerTaskTableEnd     equ 20      ;进程表当前的末尾元素位置的下一位置的下标(dword)
TaskManagerStackBase        equ 24      ;进程栈空间基址(高地址)(dword)
TaskManagerStackBufferSize  equ 28      ;进程栈空间的大小(dword)
TaskManagerStackVailedBase  equ 32      ;进程栈空间中可用地址的基址(dword)
TaskManagerTaskStackSize    equ 36      ;一个进程所占用的进程栈大小(dword)
;end struct TaskManager

;结构体大小
PROTECT_SIZE        equ 24
TASK_SIZE           equ 114
TSS_SIZE            equ 0 ;TODO
STACK_FRAME_SIZE    equ 72
TASK_MANAGER_SIZE   equ 40


;外部引用符号
extern taskManagerInstance


;导出符号
global restartProcess
global saveProcessState


[section .text]
;恢复进程的执行
;void restartProcess();
restartProcess:
    mov esp, [taskManagerInstance]
    mov esp, [esp + TaskManagerRunning]
    mov ebx, esp    ;ebx = taskManagerInstance->running;
    lldt [ebx + TaskLdtSelector]    ;ldtr = taskManagerInstance->running->ldtSelector;
    lea eax, [ebx + TaskStackFrame + STACK_FRAME_SIZE]
    mov ebx, [taskManagerInstance]
    mov ebx, [ebx + TaskManagerProtect]
    mov ebx, [ebx + ProtectTss]
    mov [ebx + TssEsp0], eax   ;taskManagerInstance->protect->tss->esp0 =
                               ;  &(taskManagerInstance->running->tackFrame) + sizeof(StackFrame);
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
    iretd

;保存进程状态，此时esp要指向进程表栈帧的eax成员
;void processState();
saveProcessState:
    pushad
    push ds
    push es
    push fs
    push gs
    jmp [esp + TaskStackFrame + StackFrameRetAddr]
