#ELF文件中代码段的基地址
TEXTBASE	= 0x30400

ASM 		= nasm
CC 			= gcc
LD 			= ld
ASMBFLAGS	= -i boot/
ASMKFLAGS 	= -I include/asm/ -f elf
CFLAGS 		= -I include/ -c -fno-builtin
LDFLAGS 		= -Ttext $(TEXTBASE)
SINGLEOUT 	= -o $@ $<
TRASHDIR 	= /home/fallenworld/trash

BOOT 		= boot/boot.bin boot/loader.bin
KERNEL 		= kernel/kernel.bin
OBJS 		= kernel/kernel.o kernel/start.o kernel/proc.o kernel/procasm.o kernel/protectmode.o \
				 kernel/interrupt.o lib/string.o lib/portio.o lib/print.o

.PHONY : all build image clean

all : clean image

image: boot.image

boot.image : $(BOOT) $(KERNEL)
	dd if=boot/boot.bin of=boot.img bs=512 count=1 conv=notrunc
	sudo mount boot.img /mnt/floppy/
	sudo cp -fv boot/loader.bin kernel/kernel.bin /mnt/floppy/
	sleep 0.1
	sudo umount /mnt/floppy/

build : $(BOOT) $(KERNEL)

# BootLoader：
boot/boot.bin : boot/boot.asm  boot/include/fat12head.inc boot/include/loader.inc
	$(ASM) $(ASMBFLAGS) $(SINGLEOUT)

boot/loader.bin : boot/loader.asm boot/fat12.asm boot/include/loader.inc boot/include/protectmode.inc \
					lib/print.asm
	$(ASM) $(ASMBFLAGS) $(SINGLEOUT)

# 内核:
kernel/kernel.bin : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

kernel/start.o : kernel/start.asm
	$(ASM) $(ASMKFLAGS) $(SINGLEOUT)

kernel/kernel.o : kernel/kernel.c include/print.h include/protectmode.h include/string.h include/type.h \
					include/kernel.h include/proc.h
	$(CC) $(CFLAGS) $(SINGLEOUT)

kernel/proc.o : kernel/proc.c include/protectmode.h include/type.h include/proc.h
	$(CC) $(CFLAGS) $(SINGLEOUT)

kernel/protectmode.o : kernel/protectmode.c include/protectmode.h include/type.h include/portio.h \
						include/interrupt.h include/print.h
	$(CC) $(CFLAGS) $(SINGLEOUT)

kernel/procasm.o : kernel/proc.asm
	$(ASM) $(ASMKFLAGS) $(SINGLEOUT)

kernel/interrupt.o : kernel/interrupt.asm
	$(ASM) $(ASMKFLAGS) $(SINGLEOUT)

# 一些库：
lib/print.o : lib/print.asm
	$(ASM) $(ASMKFLAGS) $(SINGLEOUT)

lib/portio.o : lib/portio.asm
	$(ASM) $(ASMKFLAGS) $(SINGLEOUT)

lib/string.o: lib/string.c include/string.h include/type.h
	$(CC) $(CFLAGS) $(SINGLEOUT)


clean:
	mv -fb --backup=t $(BOOT) $(KERNEL) $(OBJS) $(TRASHDIR)