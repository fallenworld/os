global outByte
global inByte

;输出一个字节到端口
;void outByte(u16 port, u8 value);
outByte:
	mov edx, [esp+4]
	mov al, [esp+8]
	out dx, al
	nop
	nop
	nop
	ret

;从端口输入一个字节
;u8 inByte(u16 port);
inByte:
	mov edx, [esp+4]
	xor eax, eax
	in al, dx
	nop
	nop
	nop
	ret