;FAT12磁盘头
	MY_BS_jmpBoot: 		jmp near FAT12_START
	MY_BS_OEMName 		db 'FallenWd'	; OEM String, 必须 8 个字节
	MY_BPB_BytsPerSec 	dw 512			; 每扇区字节数
	MY_BPB_SecPerClus 	db 1			; 每簇多少扇区
	MY_BPB_RsvdSecCnt 	dw 1			; Boot 记录占用多少扇区
	MY_BPB_NumFATs 		db 2		    ; 共有多少 FAT 表
	MY_BPB_RootEntCnt 	dw 224			; 根目录文件数最大值
	MY_BPB_TotSec16		dw 2880			; 逻辑扇区总数
	MY_BPB_Media 		db 0xf0		    ; 媒体描述符
	MY_BPB_FATSz16 		dw 9		    ; 每FAT扇区数
	MY_BPB_SecPerTrk 	dw 18			; 每磁道扇区数
	MY_BPB_NumHeads		dw 2			; 磁头数(面数)
	MY_BPB_HiddSec		dd 0		    ; 隐藏扇区数
	MY_BPB_TotSec32		dd 0			; wTotalSectorCount为0时这个值记录扇区数
	MY_BS_DrvNum 		db 0		    ; 中断13的驱动器号
	MY_BS_Reserved1		db 0			; 未使用
	MY_BS_BootSig		db 29h		    ; 扩展引导标记 (29h)
	MY_BS_VolID			dd 0		    ; 卷序列号
	MY_BS_VolLab 		db 'FallenWorld'; 卷标, 必须 11 个字节
    MY_BS_FileSysType 	db 'FAT12   '	; 文件系统类型, 必须 8个字节