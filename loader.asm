jmp START
%include 'loader.inc'
%include 'fat12.asm'
%include 'protectmode.inc'

;地址范围描述结构体
;Struct Ards
Ards_BaseAddrLow 	equ 0 	;基地址低32位(dword)
Ards_BaseAddrHigh 	equ 4 	;基地址高32位(dword)
Ards_LengthLow 		equ 8 	;长度低32位(dword)
Ards_LengthHigh		equ 12 	;长度高32位(dword)
Ards_Type 			equ 16 	;内存地址类型(dword)
;大小:
ARDS_SIZE 			equ 20 	;Ards结构体大小
;End Struct Ards

START:
	;把Kernel加载到内存
	mov ax, LOADER_SECTION_ADDR
	mov ds, ax
	mov si, kernelFileName
	mov ax, KERNEL_SECTION_ADDR
	mov es, ax
	mov bx, KERNEL_OFFSET_ADDR
	call FAT12_LOAD_FILE
	;获取内存分布信息
	mov ax, cs
	mov es, ax
	mov di, ardsBuf
	call GET_MEM_INFO
JUMP_TO_PROTECT_MODE:
	call KILL_MOTOR
	;加载GDTR
	lgdt [gdtPtr]
	;关中断
	cli
	;打开A20地址线
	in al, 92h
	or al, 00000010b
	out 92h, al
	;设置cr0
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	;执行保护模式代码段
	jmp dword SELECTOR_LOADER_C:START_PROTECT_MODE

;获取内存分布信息
;输入:es:di=存储ARDS结构体的缓冲区
;输出:ax=填充的ARDS结构体个数，若发生了错误则为0
GET_MEM_INFO:
	push ebx
	push ecx
	push edx
	push si
	push di
	push es
	mov si, 0
	mov ebx, 0
GET_AN_ARDS:
	mov eax, 0e820h
	mov ecx, 20
	mov edx, 534d4150h
	int 15h
	jc GET_ARDS_FAIL
	inc si
	add di, ARDS_SIZE
	cmp ebx, 0
	jne GET_AN_ARDS
	jmp END_GET_ARDS
GET_ARDS_FAIL:
	mov ax, 0
END_GET_ARDS:
	mov ax, si
	pop es
	pop di
	pop si
	pop edx
	pop ecx
	pop ebx
	ret

KILL_MOTOR:
	push dx
	mov dx, 03f2h
	mov al, 0
	out dx, al
	pop dx
	ret

[section .s32]
align 32
[bits 32]
%include 'print.asm'

START_PROTECT_MODE:
	;初始化寄存器;
	mov ax, SELECTOR_LOADER_RW
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	mov esp, PROTECT_MODE_LOADER_STACK_BASE
	mov ax, SELECTOR_VIDEO
	mov gs, ax
	;显示字符串
	call clearScreen
	push protectModeStr
	call printStr
	;执行kernel
	jmp $

kernelFileName db 'KERNEL     '

;全局描述符表
GDT:
DESC_EMPTY: 	Descripter 0, 		0, 		 	0	;
DESC_FLAT_C: 	Descripter 0, 		0fffffh,	DA_CR|DA_32|DA_LIMIT_4K
DESC_FLAT_RW: 	Descripter 0, 		0fffffh,	DA_DRW|DA_32|DA_LIMIT_4K
DESC_VIDEO: 	Descripter 0b8000h, 	0ffffh, 	DA_DRW|DA_DPL3
DESC_LOADER_C:	Descripter LOADER_BASE_ADDR, 0ffffh, DA_CR|DA_32
DESC_LOADER_RW:	Descripter LOADER_BASE_ADDR, 0ffffh, DA_DRW|DA_32

;GDT的大小
GDT_LEN equ $-GDT
;gdtr寄存器的值
gdtPtr 	dw GDT_LEN-1
 		dd GDT + LOADER_BASE_ADDR

;选择符
SELECTOR:
SELECTOR_FLAT_C 	equ DESC_FLAT_C-GDT
SELECTOR_FLAT_RW 	equ DESC_FLAT_RW-GDT
SELECTOR_VIDEO 		equ DESC_VIDEO-GDT
SELECTOR_LOADER_C 	equ DESC_LOADER_C-GDT
SELECTOR_LOADER_RW 	equ DESC_LOADER_RW-GDT

protectModeStr db 'Protect mode successfully entered',0

;存放ARDS的内存缓冲区
ardsBuf:
times 256 db 0

;保护模式下LOADER的栈
times 1024 db 0
PROTECT_MODE_LOADER_STACK_BASE equ $