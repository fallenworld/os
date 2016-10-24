org 07c00h
%include 'include/fat12head.inc'
%include 'include/loader.inc'
%include 'fat12.asm'

BOOT_STACK_BASE_ADDR equ 7c00h

FAT12_START:
	jmp START

START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	mov sp, BOOT_STACK_BASE_ADDR
	;初始化FAT12处理程序
	mov al, 0
	call FAT12_INIT
	;把LOADER加载到内存
	mov si, loaderFileName
	mov ax, LOADER_SECTION_ADDR
	mov es, ax
	mov bx, LOADER_OFFSET_ADDR
	call FAT12_LOAD_FILE
	;执行LOADER
	jmp LOADER_SECTION_ADDR:LOADER_OFFSET_ADDR

loaderFileName db 'LOADER  BIN'

;引导扇区格式
times 	510-($-$$)	db	0	
dw 	0xaa55