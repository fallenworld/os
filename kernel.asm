%include 'loader.inc'

START:
	mov ax, cs
	mov es, ax
	mov ax, 01301h
	mov bx, 0007h
	mov cx, 14
	mov dl, 0
    mov dh, 21
    mov bp, excutedStr
    int 10h
	jmp $

excutedStr db "Kernel excuted"