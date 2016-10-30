#include "protectmode.h"
#include "string.h"


Descriptor   gdt[GDT_LEN];
Tss          tss;


void protectModeInit()
{
    setupGdt(gdt, GDT_LEN);
    setupTss(&tss, gdt, SELECTOR_TSS / sizeof(Descriptor));
}

void setupGdt(Descriptor* gdt, int descriptorCount)
{
    memset(gdt, 0, descriptorCount * sizeof(Descriptor));
	GdtPtr gptr;
    __asm__("sgdt %0":"=m"(gptr)); 	//获取到之前gptr的值
	memcpy(gdt, (void*)gptr.base, gptr.limit+1); 	//将原来的GDT复制到新的GDT内存位置上去
	gptr.base = (u32)gdt;
	gptr.limit = descriptorCount * sizeof(Descriptor) - 1;
    __asm__("lgdt %0"::"m"(gptr)); 	//加载新的gptr
}

void setupTss(Tss* tss, Descriptor* gdt, int tssIndex)
{
    memset(tss, 0, sizeof(Tss));
	tss->ss0 = SELECTOR_FLAT_RW;
    initDescriptor(gdt + tssIndex, (u32)tss, sizeof(Tss) - 1, DA_386TSS);
    tss->ioBase = sizeof(Tss);
	//加载tr寄存器
	asm
	(
		"xor %%eax, %%eax;"
		"mov %0, %%ax;"
		"ltr %%ax"
		::"i"(SELECTOR_TSS)
	);
}

Descriptor* initDescriptor(Descriptor* descriptor, u32 base, u32 limit, u16 attribute)
{
    memset(descriptor, 0, sizeof(Descriptor));
    descriptor->limitLow = limit & 0x0ffff;
    descriptor->baseLow = base & 0x0ffff;
    descriptor->baseMid = (base >> 16) & 0x0ff;
	descriptor->attrLow = attribute & 0xff;
    descriptor->limitHighAttrHight = ((limit >> 16) & 0x0f) | (attribute >> 8) & 0xf0;
    descriptor->baseHigh = (base >> 24) & 0x0ff;
	return descriptor;
}

Gate* initGate(Gate* gate, u8 gateType, GateEntry entry, u8 privilege)
{
    memset(gate, 0, sizeof(Gate));
    u32 handlerAddr = (u32)entry;
    gate->offsetLow = handlerAddr & 0x0ffff;
	gate->selector = SELECTOR_FLAT_C;
	gate->dcount = 0;
	gate->attr = gateType | (privilege << 5);
    gate->offsetHigh = (handlerAddr >> 16) & 0x0ffff;
	return gate;
}

