%include 'print.inc'

[section .text]

global _start

_start:
	call clearScreen
	push kernelRunningStr
	call printStr
	add sp, 4
	jmp $

[section .data]
kernelRunningStr db 'Kernel entered',`\n\0`