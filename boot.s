org 07c00h

STACK_BASE equ 07c00h
ROOT_ENT_PER_SECTION equ 16
ROOT_ENT_SIZE equ 32
ROOT_START_SECTION equ 19
LOADER_FILE_NAME_LEN equ 11
FAT_TABLE_SIZE equ 4608
FAT_TABLE_SECTION_CNT equ 9
FAT_TABLE_START_SECTION equ 1
LOADER_SECTION_ADDR equ 09000h
LOADER_OFFSET_ADDR equ 0100h

;FAT12磁盘头
BS_jmpBoot:
    jmp short start
	nop
	BS_OEMName	db 'FallenWd'	; OEM String, 必须 8 个字节
	BPB_BytsPerSec	dw 512		; 每扇区字节数
	BPB_SecPerClus	db 1		; 每簇多少扇区
	BPB_RsvdSecCnt	dw 1		; Boot 记录占用多少扇区
	BPB_NumFATs	db 2		    ; 共有多少 FAT 表
	BPB_RootEntCnt	dw 224		; 根目录文件数最大值
	BPB_TotSec16	dw 2880		; 逻辑扇区总数
	BPB_Media	db 0xf0		    ; 媒体描述符
	BPB_FATSz16	dw 9		    ; 每FAT扇区数
	BPB_SecPerTrk	dw 18		; 每磁道扇区数
	BPB_NumHeads	dw 2		; 磁头数(面数)
	BPB_HiddSec	dd 0		    ; 隐藏扇区数
	BPB_TotSec32	dd 0		; wTotalSectorCount为0时这个值记录扇区数
	BS_DrvNum	db 0		    ; 中断13的驱动器号
	BS_Reserved1	db 0		; 未使用
	BS_BootSig	db 29h		    ; 扩展引导标记 (29h)
	BS_VolID	dd 0		    ; 卷序列号
	BS_VolLab	db 'FallenWorld'   ; 卷标, 必须 11 个字节
    BS_FileSysType	db 'FAT12   '	; 文件系统类型, 必须 8个字节
    
start:
    ;设置段寄存器
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STACK_BASE
    ;计算根目录区所占的扇区数(存到[rootSectionCount])
    ;数据区的起始扇区(存到[dataStart])
    ;以及根目录区大小(存到[rootSize])
    mov ax, [BPB_RootEntCnt]   ;根目录项个数
    mov bl, ROOT_ENT_PER_SECTION  ;一个扇区中根目录项的个数
    div bl
    and ah, 1
    add al, ah
    mov ah, 0
    mov [rootSectionCount], ax
    mov ax, ROOT_START_SECTION
    add ax, [rootSectionCount]
    mov [dataStart], ax
    mov ax, [rootSectionCount]
    mul word [BPB_BytsPerSec]
    mov [rootSize], ax
    ;复位软盘
    mov ah, 0
    mov dl, 0
    int 13h
    ;获取LOADER的第一个fat簇号
    call getLoaderPosition
    ;加载LOADER
    call loadLoader
    jmp $
    
;读取磁盘扇区数据到es:bx指向的内存，参数：扇区号、读扇区数
readSection:
    push bp
    mov bp, sp
    mov ax, [bp + 4]    ;要读的起始扇区
    mov cl, [BPB_SecPerTrk]
    div cl
    mov ch, al
    shr ch, 1   ;磁道号=商>>1
    mov cl, ah
    add cl, 1   ;起始扇区号=余数+1
    mov dh, al
    and dh, 1   ;磁头号=商&1
    mov dl, [BS_DrvNum] ;驱动器号
    mov al, [bp + 6]    ;要读扇区数
    mov ah, 2
    int 13h
    pop bp
    ret 4
    
;获取到loader的起始fat簇号，结果放入ax中
getLoaderPosition:
    push bx
    push si
    push di
    ;为根目录区在栈中分配空间(根目录区基址存入bx,根目录区大小存入[rootSize])
    sub sp, [rootSize]
    mov bx, sp
    ;读出根目录区
    push word [rootSectionCount]
    push ROOT_START_SECTION
    call readSection
    ;从根目录区找到LOADER
    mov cx, [BPB_RootEntCnt]   ;要检索的根目录项的个数
compareFileName:
    push cx
    mov si, bx    
    mov di, loaderFileName
    mov cx, LOADER_FILE_NAME_LEN
    repe cmpsb
    pop cx
    jz loaderFound
    add bx, 32  ;前进到下一个根目录项
    loop compareFileName
loaderNotFound:
    mov ax, 0
    push ax
    push 16
    push loaderNotFoundStr
    call printStr
    pop ax
    jmp endGetLoaderPosition
loaderFound:
    add bx, 01ah
    mov ax, [bx]
    push ax
    push 12
    push loaderFoundStr
    call printStr
    pop ax
endGetLoaderPosition:
    add sp, [rootSize]  ;根目录区出栈
    pop di
    pop si
    pop bx
    ret
    
;把loader加载到内存
loadLoader:
    ;加载fat表到栈里
    sub sp, FAT_TABLE_SIZE
    mov bx, sp
    push ax
    push FAT_TABLE_SECTION_CNT
    push FAT_TABLE_START_SECTION
    call readSection
    pop ax
    mov [fatTable], bx
    ;设置LOADER加载的位置
    mov dx, ax
    mov ax, LOADER_SECTION_ADDR
    mov es, ax
    mov ax, dx
    mov bx, LOADER_OFFSET_ADDR
loadSectionOfLoader:
    ;加载当前fat簇
    cmp ax, 0ff7h
    je badFatSection
    cmp ax, 0ff8h
    jae endLoadSectionOfLoader
    mov cx, ax
    sub cx, 2   ;第0和第1个fat簇不使用，故要减二
    add cx, [dataStart] ;当前fat簇的扇区号
    push ax
    push 1
    push cx
    call readSection
    pop ax
    add bx, 512
    ;找到下一个fat簇
    mov dx, 0
    mov cx, 2
    div cx
    and dx, 1
    mov di, dx
    add ax, dx
    sub ax, 1
    mov cx, 3
    mul cx
    mov si, ax
    add si, [fatTable]
    mov eax, [si]
    cmp di, 1
    jne evenFatNum
    shr eax, 12
evenFatNum:
    and ax, 0fffh
    jmp loadSectionOfLoader
badFatSection:
endLoadSectionOfLoader:
    mov ax, cs
    mov es, ax
    push 13
    push loaderLoadedStr
    call printStr
    add sp, FAT_TABLE_SIZE
    ret
    
;显示字符串，参数：字符串地址，字符串长度
printStr:
    push bp
    mov bp, sp
    push bx
    mov ax, 01301h
    mov bx, 0007h
    mov cx, [bp+6]
    mov dl, 0
    mov dh, [strLine]
    add byte [strLine], 1
    mov bp, [bp+4]
    int 10h
    pop bx
    pop bp
    ret 4
    
data:
    loaderFileName db 'LOADER     '
    loaderNotFoundStr db 'Loader not found'
    loaderFoundStr db 'Loader found'
    loaderLoadedStr db 'Loader loaded'
    rootSize dw 0
    rootSectionCount dw 0
    fatTable dw 0
    dataStart dw 0
    strLine db 18
                                                                                                                                                                                   
;引导扇区格式
times 	510-($-$$)	db	0	
dw 	0xaa55