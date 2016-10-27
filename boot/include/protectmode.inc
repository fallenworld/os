%ifndef OS_PROTECT_MODE_INC_
%define OS_PROTECT_MODE_INC_

;用来生成描述符的宏
%macro Descripter 3
	dw %2 & 0ffffh
	dw %1 & 0ffffh
	db (%1 >> 16) & 0ffh
	dw ((%2 >> 8) & 0f00h) | (%3 & 0f0ffh)
	db (%1 >> 24) & 0ffh
%endmacro

;描述符属性
;缩写解释:
;    DA_  : Descriptor Attribute
;    D    : 数据段
;    C    : 代码段
;    S    : 系统段
;    R    : 只读
;    RW   : 读写
;    A    : 已访问
DA_32		equ	4000h	;32位段
DA_LIMIT_4K	equ	8000h	;段界限粒度为 4K 字节
;特权级
DA_DPL0		equ	00h		;DPL = 0
DA_DPL1		equ	20h		;DPL = 1
DA_DPL2		equ	40h		;DPL = 2
DA_DPL3		equ	60h		;DPL = 3
;存储段描述符属性
DA_DR 		equ	90h		;存在的只读数据段类型值
DA_DRW 		equ	92h		;存在的可读写数据段属性值
DA_DRWA 	equ	93h		;存在的已访问可读写数据段类型值
DA_C 		equ	98h		;存在的只执行代码段属性值
DA_CR 		equ	9Ah		;存在的可执行可读代码段属性值
DA_CCO 		equ	9Ch		;存在的只执行一致代码段属性值
DA_CCOR 	equ	9Eh		;存在的可执行可读一致代码段属性值
;系统段描述符类属性
DA_LDT 		equ	82h		;局部描述符表段类型值
DA_TaskGate equ	85h		;任务门类型值
DA_386TSS 	equ	89h		;可用386任务状态段类型值
DA_386CGate equ	8Ch		;386调用门类型值
DA_386IGate	equ	8Eh		;386中断门类型值
DA_386TGate	equ	8Fh		;386陷阱门类型值

PG_P		equ	1	;页存在属性位
PG_RWR		equ	0	;R/W属性位值,读/执行
PG_RWW		equ	2	;R/W属性位值,读/写/执行
PG_USS		equ	0	;U/S属性位值,系统级
PG_USU		equ	4	;U/S属性位值,用户级

PAGE_SIZE 			equ 4096 		;一页的大小
PAGE_DIR_SIZE 		equ PAGE_SIZE	;页目录表的大小
PAGE_TABLE_SIZE 	equ PAGE_SIZE	;页表的大小
ENT_PER_TABLE 		equ 1024 		;一个页表/页目录表中的条目个数
MEM_PER_PAGE_TABLE 	equ 400000h		;一个页表对应的内存大小

%endif