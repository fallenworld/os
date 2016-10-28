LDT_LEN equ 2

;Struct Process
Process_stackFrame 		equ 0 								;保存寄存器状态的栈帧(108bytes)
Process_ldtSelector 	equ	108								;局部描述符表的选择符(word)
Process_ldt 		 	equ 110 							;局部描述符表(DESCRIPTOR_SIZE * LDT_LEN bytes)
Process_pid 			equ 110 + DESCRIPTOR_SIZE * LDT_LEN ;进程id(dword)
Process_name 			equ Process_pid + 4 				;进程名(16 bytes)
;End Struct Process

;Struct Tss
Tss_lastLink 	equ 0
Tss_esp0 		equ 4
;结构体所有的成员并未列出,只列出了前两个
;End Struct Tss

DESCRIPTOR_SIZE 	equ 4
STACK_FRAME_SIZE 	equ 108
PROCESS_SIZE 		equ Process_name + 16

global runProcess

;执行进程
;void runProcess(Process* process, Tss* tss);
runProcess:
	push ebp
	mov ebp, esp
	push ebx
	mov ebx, [ebp+8]
	lldt [ebx+Process_ldtSelector]
	lea eax, [ebx+Process_stackFrame+STACK_FRAME_SIZE]
	mov ebx, [ebp+12]
	mov [ebx+Tss_esp0], eax
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 4
	iretd
	pop ebx
	leave
	ret