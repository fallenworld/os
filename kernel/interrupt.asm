;异常
global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
;硬件中断
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15

extern exceptionHandler
extern hardwareInterruptHandler

;预定义的异常
divide_error:
	push 0xffffffff	;没有错误码
	push 0			;中断向量号=0
	jmp	exception
single_step_exception:
	push 0xffffffff	;没有错误码
	push 1			;中断向量号=1
	jmp	exception
nmi:
	push 0xffffffff	;没有错误码
	push 2			;中断向量号=2
	jmp	exception
breakpoint_exception:
	push 0xffffffff	;没有错误码
	push 3			;中断向量号=3
	jmp	exception
overflow:
	push 0xffffffff	;没有错误码
	push 4			;中断向量号=4
	jmp	exception
bounds_check:
	push 0xffffffff	;没有错误码
	push 5			;中断向量号=5
	jmp	exception
inval_opcode:
	push 0xffffffff	;没有错误码
	push 6			;中断向量号=6
	jmp	exception
copr_not_available:
	push 0xffffffff	;没有错误码
	push 7			;中断向量号=7
	jmp	exception
double_fault:
	push 8			;中断向量号=8
	jmp	exception
copr_seg_overrun:
	push 0xffffffff	;没有错误码
	push 9			;中断向量号=9
	jmp	exception
inval_tss:
	push 10			;中断向量号=A
	jmp	exception
segment_not_present:
	push 11			;中断向量号=B
	jmp	exception
stack_exception:
	push 12			;中断向量号=C
	jmp	exception
general_protection:
	push 13			;中断向量号=D
	jmp	exception
page_fault:
	push 14			;中断向量号=E
	jmp	exception
copr_error:
	push 0xffffffff	;没有错误码
	push 16			;中断向量号=10h
	jmp	exception

exception:
	call exceptionHandler
	add esp, 8
	hlt
	;iretd


;硬件中断
hwint00:
	push 0
	jmp hwinthandler
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


hwinthandler:
	call hardwareInterruptHandler
	add sp, 4
	hlt
	;iretd
