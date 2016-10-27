global _start
extern cstart

SELECTOR_FLAT_C equ 8

[section .text]
_start:
	;切换栈
	mov esp, STACK_BASE
	;进入c语言函数
	call cstart
	jmp SELECTOR_FLAT_C:LOOP
LOOP:
	sti
	hlt

[section .bss]
resb 2048
STACK_BASE:

