displayMemOffset dw 0 	;当前输出位置的显存偏移地址

BYTES_PER_ROW 	equ 160
BYTES_PER_CHAR 	equ 2

WHITE 		equ 07h
WHITE_BLINK equ 87h

;导出符号
global setPrintPosition
global clearScreen
global printHex
global printDwordHex
global printChar
global printStr

;设置输出的位置
;void setPrintPosition(uint16 offset);
;参数 offset 要设置的输出位置的显存偏移地址
setPrintPosition:
	push ebp
	mov ebp, esp
	push ebx
	mov bx, [ebp+8]
	mov [displayMemOffset], bx
	mov byte [gs:bx], '_'
	mov byte [gs:bx+1], WHITE_BLINK
	pop ebx
	leave
	ret

;清屏
;void cleanScreen();
clearScreen:
	push edi
	push es
	mov ax, gs
	mov es, ax
	mov edi, 0
	mov ecx, 7fffh
	mov al, 0
	rep stosb
	mov byte [gs:0], '_'
	mov byte [gs:1], WHITE_BLINK
	mov word [displayMemOffset], 0
	pop es
	pop edi
	ret

;将一个十六进制位输出在屏幕上
;uint16 printHex(uint8 b);
;参数b 要输出的数值，只会输出最低位的16进制位
;返回值 ax=当前屏幕上输出位置的显存偏移地址
printHex:
	push ebp
	mov ebp, esp
	mov al, [ebp+8]
	and al, 0fh
	cmp al, 9
	ja ALPHA_NUM
	add al, 30h
	jmp SHOW_HEX
ALPHA_NUM:
	add al, 37h
SHOW_HEX:
	push eax
	call printChar
	leave
	ret

;将一个32位数值的十六进制形式输出在屏幕上
;uint16 printDwordHex(uint32 value);
;参数value 要输出的数值
;返回值 ax=当前屏幕上输出位置的显存偏移地址
printDwordHex:
	push ebp
	mov ebp, esp
	mov cl, 32
PRINT_DWORD_HEX_LOOP:
	sub cl, 4
	mov eax, [ebp+8]
	shr eax, cl
	push eax
	call printHex
	cmp cl, 0
	jnz PRINT_DWORD_HEX_LOOP
	leave
	ret

;向屏幕输出一个字符
;uint16 printChar(char c);
;参数c 字符的Ascii码
;返回值 ax=当前屏幕上输出位置的显存偏移地址
printChar:
	push ebp
	mov ebp, esp
	push ebx
	push edi
	mov al, [ebp+8]
	mov di, [displayMemOffset]
	;判断是否要换行
	cmp al, 0ah
	je PRINT_CHAR_NEW_LINE
	cmp al, 0dh
	je PRINT_CHAR_NEW_LINE
	;不需要换行则显示字符
	mov byte [gs:di], al
	mov byte [gs:di+1], WHITE
	add di, 2
	jmp END_PRINT_CHAR
PRINT_CHAR_NEW_LINE:
	;换行
	mov word [gs:di], 0
	mov ax, di
	mov bl, BYTES_PER_ROW
	div bl
	mov al, ah
	mov ah, 0
	sub di, ax
	add di, BYTES_PER_ROW
END_PRINT_CHAR:
	mov [displayMemOffset], di
	mov byte [gs:di], '_'
	mov byte [gs:di+1], WHITE_BLINK
	mov eax, 0
	mov ax, di
	pop edi
	pop ebx
	leave
	ret


;向屏幕输出一个字符串
;uint16 printStr(char* str);
;参数str 字符串的地址，该字符串以/0结尾
;返回值 ax=当前屏幕上输出位置的显存偏移地址
printStr:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	mov esi, [ebp+8]	;esi=字符串地址
	mov di, [displayMemOffset]	;di=当前输出位置显存偏移
SHOW_CHAR:
	mov al, [esi]
	cmp al, 0
	je END_PRINT_STR
	;判断是否要换行
	cmp al, 0ah
	je PRINT_STR_NEW_LINE
	cmp al, 0dh
	je PRINT_STR_NEW_LINE
	;不需要换行则显示字符
	mov byte [gs:di], al
	mov byte [gs:di+1], WHITE
	inc esi
	add di, 2
	jmp SHOW_CHAR
PRINT_STR_NEW_LINE:
	;换行
	mov word [gs:di], 0
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
	mov [displayMemOffset], di
	mov byte [gs:di], '_'
	mov byte [gs:di+1], WHITE_BLINK
	mov eax, 0
	mov ax, di
	pop edi
	pop esi
	pop ebx
	leave
	ret