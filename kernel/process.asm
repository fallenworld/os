%include "const.inc"


;
;模块外部符号
;
extern tss


;
;模块内部符号
;
extern runningProcess       ;正在运行的进程的
global restartProcess
global saveProcessState


[section .text]
;恢复进程的执行
;void restartProcess();
restartProcess:
    mov esp, [runningProcess]
    lldt [esp+Process_ldtSelector]
    lea eax, [esp+Process_stackFrame+STACK_FRAME_SIZE]
    mov [tss+Tss_esp0], eax
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
    jmp [esp+Process_stackFrame+StackFrame_retAddr]
