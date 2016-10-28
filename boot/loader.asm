jmp START
%include 'include/loader.inc'
%include 'fat12.asm'

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
	mov [ardsCount], ax
JUMP_TO_PROTECT_MODE:
	call KILL_MOTOR
	;加载GDTR
	lgdt [gdtPtr]
	lidt [idtPtr]
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
	xor si, si
	xor ebx, ebx
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
	xor ax, ax
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
	xor al, al
	out dx, al
	pop dx
	ret

[section .s32]
align 32
[bits 32]
%include '../lib/print.asm'

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
	add sp, 4
	;显示可用内存信息并获取最大可用内存地址
	call SHOW_MEM_INFO	;eax=最大可用内存地址
	;启动分页
	call ENABLE_PAGE
	push pageEnabledStr
	call printStr
	add sp, 4
	;将内核文件的段放到正确位置上,并获取到内核入口地址(eax=内核入口地址)
	call SET_KERNEL_ELF
	;设置段寄存器，执行kernel
	jmp SELECTOR_FLAT_C:(RUN_KERNEL+LOADER_BASE_ADDR)
RUN_KERNEL:
	mov ebx, eax
	mov ax, SELECTOR_FLAT_RW
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov ax, SELECTOR_VIDEO
	mov gs, ax
	mov eax, ebx
	jmp eax

;显示可用内存信息并获取最大可用内存地址
;输出eax=最大可用内存地址
SHOW_MEM_INFO:
	xor ecx, ecx
	mov cx, [ardsCount]
	xor eax, eax	;eax=最大可用内存
	mov ebx, ardsBuf
SHOW_AN_ARDS:
	mov edx, [ebx+Ards_Type]
	cmp edx, 1
	jne NEXT_ARDS
	mov edx, [ebx+Ards_BaseAddrLow]
	push ecx
	push edx
	call printDwordHex
	push '-'
	call printChar
	add sp, 4
	pop edx
	add edx, [ebx+Ards_LengthLow]
	push edx
	call printDwordHex
	push `\n`
	call printChar
	add sp, 4
	pop edx
	pop ecx
	cmp edx, eax
	jna NEXT_ARDS
	mov eax, edx
NEXT_ARDS:
	add ebx, ARDS_SIZE
	loop SHOW_AN_ARDS
	ret

;启动分页
;这里令线性地址=物理地址
;输入eax=最大内存地址
ENABLE_PAGE:
	push es
	push eax
	mov ax, SELECTOR_FLAT_RW
	mov es, ax
	pop eax
	;计算页表个数
	xor edx, edx
	mov ecx, MEM_PER_PAGE_TABLE
	div ecx
	and edx, 1
	add eax, edx 	;eax=页表个数
	;初始化页目录表
	push eax
	mov ecx, eax	;ecx=要初始化的页表项个数
	mov eax, PAGE_TABLE_BASE|PG_P|PG_USU|PG_RWW
	mov edi, PAGE_DIR_BASE
SET_PAGE_DIR:
	stosd
	add eax, PAGE_TABLE_SIZE
	loop SET_PAGE_DIR
	;初始化页表
	pop eax
	mov cx, ENT_PER_TABLE
	mul cx	;乘法运算完成之后，eax=要初始化的页表项个数
	mov ecx, eax
	mov eax, 0|PG_P|PG_USU|PG_RWW
	mov edi, PAGE_TABLE_BASE
SET_PAGE_TABLE:
	stosd
	add eax, PAGE_SIZE
	loop SET_PAGE_TABLE
	;设置cr0和cr3，启动分页
	mov eax, PAGE_DIR_BASE
	mov cr3, eax
	mov eax, cr0
	or eax, 80000000h
	mov cr0, eax
	;清空流水线
	jmp short END_ENABLE_PAGE
END_ENABLE_PAGE:
	nop
	pop es
	ret

;ELF文件头
;Struct ElfHeader
ElfHeader_ident 	equ 0 	;文件标示字节(16bytes)
ElfHeader_type 		equ 16 	;文件类型(word)
ElfHeader_machine 	equ 18 	;程序运行的体系结构(word)
ElfHeader_version 	equ 20 	;文件版本(dword)
ElfHeader_entry 	equ 24 	;程序入口地址(dword)
ElfHeader_phoff 	equ 28 	;程序头表在文件中偏移(dword)
ElfHeader_shoff 	equ 32 	;段表在文件中偏移(dword)
ElfHeader_flags 	equ 36 	;对IA32而言，此项位0(dword)
ElfHeader_ehsize 	equ 40 	;ELF头的大小(word)
ElfHeader_phentsize equ 42 	;程序头表中一个程序头的大小(word)
ElfHeader_phnum 	equ 44 	;程序头表中有多少个条目(word)
ElfHeader_shentsize equ 46 	;段表中一个条目的大小(word)
ElfHeader_shnum 	equ 48 	;段表中的条目个数(word)
ElfHeader_shstrndx 	equ 50 	;字符串表的段条目在段表中的下标(word)
;大小:
ELF_HEADER_SIZE 	equ 52 	;ELF头结构体的大小
;End Struct ElfHead

;ELF程序头，用来描述加载的段
;Struct ProgramHeader
ProgramHeader_type 		equ 0 	;段的类型(dword)
ProgramHeader_offset 	equ 4 	;段在文件中的偏移(dword)
ProgramHeader_vaddr 	equ 8 	;段的加载地址(dword)
ProgramHeader_paddr 	equ 12 	;为物理地址保留(dword)
ProgramHeader_filesz 	equ 16 	;段在文件中的长度(dword)
ProgramHeader_memsz 	equ 20 	;段在内存中的长度(dword)
ProgramHeader_flags 	equ 24 	;和段相关的标志(dword)
ProgramHeader_align 	equ 28 	;对齐方式(dword)
;大小:
PROGRAM_HEADER_SIZE 	equ 32 	;ProgramHeader结构体的大小
;End Struct ElfHead

;读取内核的ELF头，将相应的段放到正确的内存地址上,并获取到入口地址
;输出eax=程序入口地址
SET_KERNEL_ELF:
	push ds
	push es
	;设置段寄存器
	mov ax, SELECTOR_FLAT_RW
	mov ds, ax
	mov es, ax
	;读取ELF头，获取到程序头表的偏移、程序头表条目个数、程序入口地址
	mov ebx, KERNEL_BASE_ADDR				;ebx=ElfHeader的基地址
	mov eax, [ebx+ElfHeader_entry] 			;eax=程序入口地址
	mov ecx, [ebx+ElfHeader_phnum]
	and ecx, 00ffh 							;ecx=程序头表中条目个数
	add ebx, [ebx+ElfHeader_phoff] 			;ebx=程序头表的地址
	;复制段到相应内存地址上去
LOAD_A_SEGMENT:
	push ecx
	mov ecx, [ebx+ProgramHeader_filesz] 	;ecx=段的大小
	mov esi, KERNEL_BASE_ADDR
	add esi, [ebx+ProgramHeader_offset] 	;esi=被复制的段的源地址
	mov edi, [ebx+ProgramHeader_vaddr] 		;edi=被复制的段的目的地址
	rep movsb
	add ebx, PROGRAM_HEADER_SIZE 	;前进到下一个程序头表项
	pop ecx
	loop LOAD_A_SEGMENT
	pop es
	pop ds
	ret



kernelFileName db 'KERNEL  BIN'
protectModeStr db 'Protect mode successfully entered',`\n\n`,'Available memory:',`\n\0`
pageEnabledStr db `\n`,'Memory paging enabled',`\n\0`

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
;idtr寄存器的值（在kernel再去设置实际的值）
idtPtr  dw 0
		dd 0

;选择符
SELECTOR:
SELECTOR_EMPTY 		equ 0
SELECTOR_FLAT_C 	equ DESC_FLAT_C-GDT
SELECTOR_FLAT_RW 	equ DESC_FLAT_RW-GDT
SELECTOR_VIDEO 		equ DESC_VIDEO-GDT
SELECTOR_LOADER_C 	equ DESC_LOADER_C-GDT
SELECTOR_LOADER_RW 	equ DESC_LOADER_RW-GDT


;存放ARDS的内存缓冲区
ardsBuf:
times 256 db 0

ardsCount dw 0

;保护模式下LOADER的栈
times 1024 db 0
PROTECT_MODE_LOADER_STACK_BASE equ $
