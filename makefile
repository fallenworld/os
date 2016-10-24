#ELF文件中代码段的基地址
TEXTBASE	= 0x30400

ASM 		= nasm
CC 			= gcc
LD 			= ld
ASMBFLAG	= -i boot/
ASMKFLAG 	= -I include/ -f elf
CFLAG 		= -I include/ -c -fno-bultin
LDFLAG 		= -Ttext $(TEXTBASE)
SINGLEOUT 	= -o $@ $<
TRASHDIR 	= /home/fallenworld/trash

BOOT 		= boot/boot.bin boot/loader.bin
KERNEL 		= kernel/kernel.bin
OBJS 		= kernel/kernel.o lib/print.o

.PHONY : all build image

all : clean build image

image : boot.img

build : $(BOOT) $(KERNEL)

boot.img : $(BOOT) $(KERNEL)
	sudo cp -fv boot/loader.bin /mnt/floppy/
	sudo cp -fv kernel/kernel.bin /mnt/floppy/

boot/boot.bin : boot/boot.asm  boot/include/fat12head.inc boot/include/loader.inc
	$(ASM) $(ASMBFLAG) $(SINGLEOUT)

boot/loader.bin : boot/loader.asm boot/fat12.asm boot/include/loader.inc boot/include/protectmode.inc \
					lib/print.asm
	$(ASM) $(ASMBFLAG) $(SINGLEOUT)

kernel/kernel.bin : $(OBJS)
	$(LD) $(LDFLAG) -o $@ $(OBJS)

kernel/kernel.o : kernel/kernel.asm include/print.inc
	$(ASM) $(ASMKFLAG) $(SINGLEOUT)

lib/print.o : lib/print.asm
	$(ASM) $(ASMKFLAG) $(SINGLEOUT)

clean:
	mv -fb --backup=t $(BOOT) $(KERNEL) $(OBJS) $(TRASHDIR)