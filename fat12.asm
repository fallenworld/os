;常量定义
FAT12_ROOT_ENT_PER_SECTION 	equ 16		;一个扇区中根目录项的个数
FAT12_ROOT_START_SECTION 	equ 19		;根目录区的起始扇区号
FAT12_ROOT_FILE_NAME_LEN 	equ 11		;根目录项中文件名的长度
FAT12_SECTION_SIZE 			equ 512		;一个扇区的大小
FAT12_TABLE_SIZE 			equ 4608	;FAT表的大小
FAT12_TABLE_SECTION_CNT 	equ 9		;FAT表所占扇区数
FAT12_TABLE_START_SECTION 	equ 1		;FAT表的起始扇区号

;========结构体定义========;

;FAT12磁盘信息
;Struct FAT12Info
FAT12Info_RootSize 			equ	0 	;根目录区的大小(word)
FAT12Info_RootSectionCount 	equ 2	;根目录区所占的扇区数(word)
FAT12Info_DataStart 		equ 4	;数据区的起始扇区号(word)
;End Struct FAT12Info

;FAT12头
;Struct FAT12Head
BS_JmpBoot	 	equ 0 	;跳转指令(3 bytes)
BS_OEMName 		equ 3	;OEM String,必须8个字节(8 bytes)
BPB_BytsPerSec 	equ 11	;每扇区字节数(word)
BPB_SecPerClus 	equ 13	;每簇多少扇区(byte)
BPB_RsvdSecCnt 	equ 14	;Boot记录占用多少扇区(word)
BPB_NumFATs 	equ 16	;共有多少FAT表(byte)
BPB_RootEntCnt 	equ 17	;根目录文件数最大值(word)
BPB_TotSec16	equ 19	;辑扇区总数(word)
BPB_Media 		equ 21	;媒体描述符(byte)
BPB_FATSz16 	equ 22	;每FAT扇区数(word)
BPB_SecPerTrk 	equ 24	;每磁道扇区数(word)
BPB_NumHeads	equ 26	;磁头数(面数)(word)
BPB_HiddSec		equ 28	;隐藏扇区数(double word)
BPB_TotSec32	equ 32	;wTotalSectorCount为0时这个值记录扇区数(double word)
BS_DrvNum 		equ 36	;中断13的驱动器号(byte)
BS_Reserved1	equ 37	;未使用(byte)
BS_BootSig		equ 38	;扩展引导标记(29h)(byte)
BS_VolID		equ 39	;卷序列号(double word)
BS_VolLab 		equ 43	;卷标, 必须 11 个字节(11 bytes)
BS_FileSysType 	equ 54	;文件系统类型, 必须 8个字节(8 bytes)
;End Struct FAT12Head

;根目录项
;Struct FAT12RootEntry
FAT12RootEntry_Name 		equ 0	;文件名(11 bytes)
FAT12RootEntry_Attribute 	equ 0bh	;文件属性(byte)
FAT12RootEntry_Reserve 		equ 0ch	;保留位(10 bytes)
FAT12RootEntry_WriteTime 	equ 16h	;最后一次写入时间(word)
FAT12RootEntry_WriteDate 	equ 18h	;最后一次写入日期(word)
FAT12RootEntry_FirstClus 	equ 1ah	;文件起始FAT簇号(word)
FAT12RootEntry_FileSize 	equ 1ch	;文件大小(double word)
;End Struct FAT12RootEntry

;结构体大小
FAT12_INFO_SIZE 		equ 6	;FAT12Info结构体大小
FAT12_HEAD_SIZE 		equ 62	;FAT12Head结构体大小
FAT12_ROOT_ENTRY_SIZE 	equ 32	;FAT12RootEntry结构体大小

;常量地址定义
FAT12_DATA_SECTION 	equ 02000h						;FAT12程序数据段地址
FAT12_HEAD 			equ 0							;FAT12头地址
FAT12_INFO 			equ FAT12_HEAD+FAT12_HEAD_SIZE	;FAT12磁盘信息地址
FAT12_TABLE 		equ FAT12_INFO+FAT12_INFO_SIZE	;FAT表地址
FAT12_ROOT 			equ FAT12_TABLE+FAT12_TABLE_SIZE;根目录区地址

;初始化FAT12磁盘处理
;输入:al=磁盘号
;输出:无
FAT12_INIT:
	push ax
	push bx
	push cx
	push dx
	push di
	push si
	push ds
	push es
	push ax
	mov ax, FAT12_DATA_SECTION
	mov ds, ax
	pop ax
	;复位软盘
	mov ah, 0
	mov dl, al
	int 13h
	;读取FAT12第一个扇区
	mov dl, al
	mov dh, 0
	mov cl, 1
	mov ch, 0
	mov ax, ss
	mov es, ax
	sub sp, FAT12_SECTION_SIZE
	mov bx, sp
	mov al, 1
	mov ah, 2
	int 13h
	;获取到FAT12磁盘头
	mov cx, FAT12_HEAD_SIZE
	push ds
	mov ax, es
	mov ds, ax
	mov si, bx	;ds:si=es:bx=ss:sp
	mov ax, FAT12_DATA_SECTION
	mov es, ax	
	mov di, FAT12_HEAD 	;es:di=FAT12_DATA_SECTION:FAT12_HEAD
	rep movsb
	pop ds
	;初始化FAT12磁盘信息
	mov ax, [FAT12_HEAD+BPB_RootEntCnt]	;ax=根目录项个数
	mov bl, FAT12_ROOT_ENT_PER_SECTION 	;bl=一个扇区中根目录项的个数
	div bl
	and ah, 1
	add al, ah
	mov ah, 0
	mov [FAT12_INFO+FAT12Info_RootSectionCount], ax	;14
	mov ax, FAT12_ROOT_START_SECTION
	add ax, [FAT12_INFO+FAT12Info_RootSectionCount]
	mov [FAT12_INFO+FAT12Info_DataStart], ax
	mov ax, [FAT12_INFO+FAT12Info_RootSectionCount]
	mul word [BPB_BytsPerSec]
	mov [FAT12_INFO+FAT12Info_RootSize], ax
	;加载fat表
	push ax	;暂存ax
	push bx	;暂存bx
	mov bx, FAT12_TABLE
	mov ah, FAT12_TABLE_SECTION_CNT
	mov al, FAT12_TABLE_START_SECTION
	call FAT12_READ_SECTION
	pop bx	;恢复ax
	pop ax	;恢复bx
	;加载根目录区
	mov bx, FAT12_ROOT
	mov ah, [FAT12_INFO+FAT12Info_RootSectionCount]
	mov al, FAT12_ROOT_START_SECTION
	call FAT12_READ_SECTION
	add sp, FAT12_SECTION_SIZE
	pop es
	pop ds
	pop si
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	ret

;读取磁盘扇区数据
;输入:ah=读扇区数
;    al=起始扇区号
;    es:bx=保存扇区数据的缓冲地址
;输出:无
FAT12_READ_SECTION:
	push ax
	push cx
	push dx
	push di
	push ds
	push ax
	mov ax, FAT12_DATA_SECTION
	mov ds, ax
	pop ax
	mov di, bx
	mov bh, ah
	mov ah, 0
	mov cl, [FAT12_HEAD+BPB_SecPerTrk]
	div cl
	mov ch, al
	shr ch, 1   ;ch=磁道号=商>>1
	mov cl, ah
	add cl, 1   ;cl=起始扇区号=余数+1
	mov dh, al
	and dh, 1   ;dh=磁头号=商&1
	mov dl, [FAT12_HEAD+BS_DrvNum] ;dl=驱动器号
	mov al, bh  ;al=要读扇区数
	mov ah, 2
	mov bx, di
	int 13h
	pop ds
	pop di
	pop dx
	pop cx
	pop ax
	ret

;获取到文件的起始fat簇号
;输入:ds:si=文件名字符串的地址
;输出:ax=获取到的文件fat簇号,没找到文件则为0
FAT12_GET_CLUS:
	push bx
	push cx
	push dx
	push di
	push si
	push ds
	push es
	push ax
	mov ax, FAT12_DATA_SECTION
	mov es, ax
	pop ax
FAT12_ROOT_LOADED:
	;从根目录区找到文件
	mov cx, [es:FAT12_HEAD+BPB_RootEntCnt]   ;cx=要检索的根目录项的个数
	mov bx, FAT12_ROOT
FAT12_COMPARE_FILE_NAME:
	push cx
	push si
	mov di, bx
	mov cx, FAT12_ROOT_FILE_NAME_LEN
	repe cmpsb
	pop si
	pop cx
	jz FAT12_FILE_FOUND
	add bx, FAT12_ROOT_ENTRY_SIZE  ;前进到下一个根目录项
	loop FAT12_COMPARE_FILE_NAME
FAT12_FILE_NOT_FOUND:
	mov ax, 0
	jmp FAT12_END_GET_CLUS
FAT12_FILE_FOUND:
	mov ax, [es:bx+FAT12RootEntry_FirstClus]
FAT12_END_GET_CLUS:
	pop es
	pop ds
	pop si
	pop di
	pop dx
	pop cx
	pop bx
	ret

;根据文件的fat簇号把文件加载到内存
;输入:ax=文件的第一个fat簇号
;    es:bx加载文件的地址
;输出:无
FAT12_LOAD_FILE_BY_CLUS:
	push eax
	push bx
	push cx
	push dx
	push di
	push si
	push ds
	push ax
	mov ax, FAT12_DATA_SECTION
	mov ds, ax
	pop ax
FAT12_LOAD_SECTION_OF_FILE:
	;加载当前fat簇
	cmp ax, 0ff7h
	je FAT12_BAD_FAT_SECTION
	cmp ax, 0ff8h
	jae FAT12_END_LOAD_SECTION_OF_FILE
	push ax
	sub ax, 2   ;第0和第1个fat簇不使用，故要减二
	add ax, [FAT12_INFO+FAT12Info_DataStart] ;ax=当前fat簇的扇区号
	mov ah, 1
	call FAT12_READ_SECTION
	pop ax
	add bx, FAT12_SECTION_SIZE
	;找到下一个fat簇
	mov dx, 0
	mov cx, 2
	div cx
	mov di, dx
	mov cx, 3
	mul cx
	mov si, ax
	add si, FAT12_TABLE
	mov eax, [si]
	cmp di, 1
	jne FAT12_EVEN_FAT_NUM
	shr eax, 12
FAT12_EVEN_FAT_NUM:
	and ax, 0fffh
	jmp FAT12_LOAD_SECTION_OF_FILE
FAT12_BAD_FAT_SECTION:
FAT12_END_LOAD_SECTION_OF_FILE:
	pop ds
	pop si
	pop di
	pop dx
	pop cx
	pop bx
	pop eax
	ret

;根据文件的文件名把文件加载到内存
;输入:ds:si=文件名字符串的地址
;    es:bx加载文件的地址
;输出:无
FAT12_LOAD_FILE:
FAT12_LOAD_FILE_BY_NAME:
	call FAT12_GET_CLUS
	call FAT12_LOAD_FILE_BY_CLUS
	ret