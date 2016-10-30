;8259A端口
INT_M_CTL       equ 0x20
INT_M_CTLMASK   equ 0x21
INT_S_CTL       equ 0xa0
INT_S_CTLMASK   equ 0xa1
;8259A控制字
EOI equ 0x20

;任务状态段
;Struct Tss
Tss_lastLink 	equ 0
Tss_esp0        equ 4
;结构体所有的成员并未列出,只列出了前两个s
;End Struct Tss

;进程
;Struct Process
Process_stackFrame 	equ 0                               ;保存寄存器状态的栈帧(72bytes)
Process_ldtSelector     equ 72                              ;局部描述符表的选择符(word)
Process_ldt             equ 74                              ;局部描述符表(DESCRIPTOR_SIZE * LDT_LEN bytes)
Process_pid 		equ 74 + DESCRIPTOR_SIZE * LDT_LEN  ;进程id(dword)
Process_name 		equ Process_pid + 4                 ;进程名(16 bytes)
PROCESS_SIZE 		equ Process_name + 16
;End Struct Process

;栈帧
;Struct StackFrame
StackFrame_gs           equ 0
StackFrame_fs           equ 4
StackFrame_es           equ 8
StackFrame_ds           equ 12
StackFrame_edi          equ 16
StackFrame_esi          equ 20
StackFrame_ebp          equ 24
StackFrame_kernel_esp   equ 28
StackFrame_ebx          equ 32
StackFrame_edx          equ 36
StackFrame_ecx          equ 40
StackFrame_eax          equ 44
StackFrame_retAddr      equ 48
StackFrame_eip          equ 52
StackFrame_cs           equ 56
StackFrame_eflags       equ 60
StackFrame_esp          equ 64
StackFrame_ss           equ 68
STACK_FRAME_SIZE 	equ 72
;End Struct StackFrame

DESCRIPTOR_SIZE equ 4   ;描述符大小
LDT_LEN         equ 2   ;局部描述符表的长度

;选择符
SELECTOR_FLAT_C     equ 0x08    ;代码段
SELECTOR_FLAT_RW    equ 0x10    ;数据段
SELECTOR_VIDEO      equ 0x18    ;显存
SELECTOR_TSS        equ 0x20    ;任务状态段


;系统调用相关
SYSCALL_INT_VECTOR  equ 0x80    ;系统调用的中断向量号



