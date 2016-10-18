org 07c00h
%include 'fat12head.inc'
%include 'fat12.asm'

LOADER_SECTION_ADDR equ 9000h
LOADER_OFFSET_ADDR	equ 0100h

FAT12_START:
	jmp START

START:
	;初始化FAT12处理程序
	mov al, 0
	call FAT12_INIT
	;把LOADER加载到内存
	mov si, LOADER
	mov ax, LOADER_SECTION_ADDR
	mov es, ax
	mov bx, LOADER_OFFSET_ADDR
	call FAT12_LOAD_FILE
	;执行LOADER
	jmp LOADER_SECTION_ADDR:LOADER_OFFSET_ADDR

LOADER db 'LOADER     '


;引导扇区格式
times 	510-($-$$)	db	0	
dw 	0xaa55