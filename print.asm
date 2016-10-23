printPosition:
printPositionX db 0	;当前屏幕上的输出位置行数(0为起始)
printPositionY db 0 ;当前屏幕上的输出位置列数(0为起始)

BYTES_PER_ROW 	equ 160
BYTES_PER_CHAR 	equ 2

WHITE 	equ 07h

;设置输出的位置
;void setPrintPosition(word position);
;参数 position 要设置的输出位置
setPrintPosition:
	push ebp
	mov ebp, esp
	mov ax, [ebp+8]
	mov [printPosition], ax
	pop ebp
	ret

;保护模式下向屏幕输出一个字符串
;word printStr(char* str);
;参数str 字符串的地址，该字符串以/0结尾
;返回值 ax=当前屏幕上的输出位置
printStr:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	mov esi, [ebp+8]	;esi=字符串地址
	;设置显存偏移地址初始值
	mov di, 0
	mov al, BYTES_PER_ROW
	mul byte [printPositionY]
	add di, ax
	mov al, BYTES_PER_CHAR
	mul byte [printPositionX]
	add di, ax
SHOW_CHAR:
	mov al, [esi]
	cmp al, 0
	je END_PRINT_STR
	;判断是否要换行
	cmp al, 0ah
	je NEW_LINE
	cmp al, 0dh
	je NEW_LINE
	;不需要换行则显示字符
	mov byte [gs:di], al
	mov byte [gs:di+1], WHITE
	inc esi
	add di, 2
	jmp SHOW_CHAR
NEW_LINE:
	;换行
	inc esi
	mov ax, di
	mov bl, BYTES_PER_ROW
	div bl
	mov al, ah
	mov ah, 0
	sub di, ax
	add di, BYTES_PER_ROW
	jmp SHOW_CHAR
END_PRINT_STR:
	mov ax, [printPosition]
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret


;清屏
;void cleanScreen();
clearScreen:
	push es
	mov ax, gs
	mov es, ax
	mov edi, 0
	mov ecx, 7fffh
	mov al, 0
	rep stosb
	pop es
	ret