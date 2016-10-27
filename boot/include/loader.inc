%ifndef OS_LOADER_INC_
%define OS_LOADER_INC_


%include 'include/protectmode.inc'

LOADER_SECTION_ADDR 	equ 9000h 	;LOADER的段地址
LOADER_OFFSET_ADDR 		equ 0	 	;LOADER的偏移地址
KERNEL_SECTION_ADDR 	equ 8000h 	;内核开始被加载到的段地址
KERNEL_OFFSET_ADDR 		equ 0 		;内核开始被加载到的偏移地址
KERNEL_BASE_ADDR		equ KERNEL_SECTION_ADDR*10h ;内核开始被加载到地址的在保护模式下的实际地址
LOADER_BASE_ADDR 		equ LOADER_SECTION_ADDR*10h	;LOADER被加载到的物理地址

KERNEL_ENTRY 	equ 00030400h 	;内核的入口地址

PAGE_DIR_BASE 	equ 100000h ;页目录表基地址
PAGE_TABLE_BASE equ PAGE_DIR_BASE+PAGE_DIR_SIZE ;页表基地址(所有页表连续存放)

%endif