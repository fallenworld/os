extern main

global _start
global kernelStack

[section .text]
_start:
	;切换栈
	mov esp, kernelStack
	;进入c语言函数
	call main 
	hlt

[section .bss]
;内核栈
resb 2048
kernelStack:

