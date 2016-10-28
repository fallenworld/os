#include "protectmode.h"
#include "portio.h"
#include "interrupt.h"
#include "print.h"

void setupGdt(Descriptor* gdt, int descriptorCount)
{
	GdtPtr gptr;
    asm("sgdt %0":"=m"(gptr)); 	//获取到之前gptr的值
	memcpy(gdt, (void*)gptr.base, gptr.limit+1); 	//将原来的GDT复制到新的GDT内存位置上去
	gptr.base = (u32)gdt;
	gptr.limit = descriptorCount * sizeof(Descriptor) - 1;
    asm("lgdt %0"::"m"(gptr)); 	//加载新的gptr
}

void setupIdt(Gate* idt, int gateCount)
{
	//初始化8259A
	init8259A();
	/* 初始化中断向量表 */
	//预定义的内部中断
	initGate(idt + INT_VECTOR_DIVIDE, 		DA_386IGate, divide_error, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_DEBUG, 		DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_NMI, 			DA_386IGate, nmi, 					PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpoint_exception, 	PRIVILEGE_USER);
	initGate(idt + INT_VECTOR_OVERFLOW, 	DA_386IGate, overflow, 				PRIVILEGE_USER);
	initGate(idt + INT_VECTOR_BOUNDS, 		DA_386IGate, bounds_check, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_INVAL_OP, 	DA_386IGate, inval_opcode, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_COPROC_NOT, 	DA_386IGate, copr_not_available, 	PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_COPROC_SEG, 	DA_386IGate, copr_seg_overrun, 		PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_INVAL_TSS, 	DA_386IGate, inval_tss, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_SEG_NOT, 		DA_386IGate, segment_not_present, 	PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_STACK_FAULT, 	DA_386IGate, stack_exception, 		PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_PROTECTION, 	DA_386IGate, general_protection, 	PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_PAGE_FAULT, 	DA_386IGate, page_fault, 			PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_COPROC_ERR, 	DA_386IGate, copr_error, 			PRIVILEGE_KRNL);
	//外部中断
	initGate(idt + INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
	initGate(idt + INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
	/* 设置idtr寄存器 */
    IdtPtr idtr;
	idtr.base = (u32)idt;
	idtr.limit = gateCount * sizeof(Gate) - 1;
    asm("lidt %0"::"m"(idtr)); 	//加载idtr
}

void setupTss(Tss* tss, Descriptor* gdt, int tssIndex)
{
	memset(tss, 0, sizeof(tss));
	tss->ss0 = SELECTOR_FLAT_RW;
    initDescriptor(gdt + tssIndex, (u32)&tss, sizeof(tss) - 1, DA_386TSS);
	tss->ioBase = sizeof(tss);
	//加载tr寄存器
	asm
	(
		"xor %%eax, %%eax;"
		"mov %0, %%ax;"
		"ltr %%ax"
		::"i"(SELECTOR_TSS)
	);
}

void init8259A()
{
	outByte(INT_M_CTL, 0x11);
	outByte(INT_S_CTL, 0x11);
	outByte(INT_M_CTLMASK, INT_VECTOR_IRQ0);
	outByte(INT_S_CTLMASK, INT_VECTOR_IRQ8);
	outByte(INT_M_CTLMASK, 0x4);
	outByte(INT_S_CTLMASK, 0x2);
	outByte(INT_M_CTLMASK, 0x1);
	outByte(INT_S_CTLMASK, 0x1);
	outByte(INT_M_CTLMASK, 0xfd);
	outByte(INT_S_CTLMASK, 0xff);
}

Descriptor* initDescriptor(Descriptor* descriptor, u32 base, u32 limit, u16 attribute)
{
	descriptor->limitLow = limit & 0xffff;
	descriptor->baseLow = base & 0xffff;
	descriptor->baseMid = (base >> 16) & 0xff;
	descriptor->attrLow = attribute & 0xff;
	descriptor->limitHighAttrHight = ((limit >> 16) & 0x0f) | (attribute >> 8) & 0xf0;
	descriptor->baseHigh = (base >> 24) & 0xff;
	return descriptor;
}

Gate* initGate(Gate* gate, u8 gateType, InterruptHandler handler, u8 privilege)
{
	u32 handlerAddr = (u32)handler;
	gate->offsetLow = handlerAddr & 0xffff;
	gate->selector = SELECTOR_FLAT_C;
	gate->dcount = 0;
	gate->attr = gateType | (privilege << 5);
	gate->offsetHigh = (handlerAddr >> 16) & 0xffff;
	return gate;
}

void exceptionHandler(int vectorNum, int errorCode, int eip, int cs, int eflag)
{
	char* errorMsg[] = 
	{
		" DE Devide error",
		" # DB RESERVED",
		"--  NMI Interrupt",
		" #BP Breakpoint",
		" #OF Overflow",
		" #BR Bound range exceeded",
		" #UD Invalid opcode (undefined opcode)",
		" #NM Device Not Available (no math coprocessor)",
		" #DF Double Fault",
		"     Coprocessor segment overrun (reserved)",
		" #TS Invalid TSS",
		" #NP Segment not present",
		" #SS Stack-segment fault",
		" #GP General protection",
		" #PF Page fault",
		" --  (Intel reserved. Do not use.)",
		" #MF x87 FPU floating-point error (math fault)",
		" #AC Alignment check",
		" #MC Machine check",
		" #XF SIMD floating-point exception"
	};
	printStr(" Exception at ");
	printWordHex(cs);
	printChar(':');
	printDwordHex(eip);
	printStr("\n ");
	printByteHex(vectorNum);
	printStr(":");
	printStr(errorMsg[vectorNum]);
	if (errorCode != 0xffffffff)
	{
		printStr(" , Error Code:");
		printDwordHex(errorCode);
	}
	printChar('\n');
}

void hardwareInterruptHandler(int code)
{
	printStr( "IRQ: ");
	printByteHex((u8)code);
	printChar('\n');
}