all:boot loader kernel
	dd if=boot of=boot.img bs=512 count=1 conv=notrunc

boot:boot.asm fat12head.inc fat12.asm  loader.inc
	nasm boot.asm
loader:loader.asm loader.inc fat12.asm protectmode.inc print.asm
	nasm loader.asm
kernel:kernel.asm loader.inc
	nasm kernel.asm

clean:
	rm -rf boot loader kernel fat12 print