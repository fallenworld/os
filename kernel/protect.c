#include "protect.h"
#include "macros.h"
#include "string.h"



#define GDT_LEN 128


/* * * 模块内部函数声明 * * */
int protectGdtInit(Protect* protect);
int protectTssInit(Protect* protect);


PUBLIC int protectInit(Protect* protect, Descriptor* gdtBuffer, int gdtLen, Tss* tss)
{
    memset(protect, 0, sizeof(Protect));
    protect->gdt = gdtBuffer;
    protect->gdtLen = gdtLen;
    protect->tss = tss;
    if (!protectGdtInit(protect))
    {
        return 0;
    }
    if (!protectTssInit(protect))
    {
        return 0;
    }
    return 1;
}

PUBLIC int protectAddDescriptor(Protect* protect, Descriptor* descriptor)
{
    if (protect->gdtEnd < protect->gdtLen)
    {
        int ret = protect->gdtEnd;
        protect->gdt[protect->gdtEnd] = *descriptor;
        protect->gdtEnd++;
        return ret;
    }
    return -1;
}

PUBLIC int descriptorInit(Descriptor* descriptor, u32 base, u32 limit, u16 attribute)
{
    memset(descriptor, 0, sizeof(Descriptor));
    descriptor->limitLow = limit & 0x0ffff;
    descriptor->baseLow = base & 0x0ffff;
    descriptor->baseMid = (base >> 16) & 0x0ff;
    descriptor->attrLow = attribute & 0xff;
    descriptor->limitHighAttrHight = ((limit >> 16) & 0x0f) | (attribute >> 8) & 0xf0;
    descriptor->baseHigh = (base >> 24) & 0x0ff;
    return 1;
}

PUBLIC int gateInit(Gate* gate, u16 selector, u8 gateType, void (*entry)(), u8 privilege)
{
    memset(gate, 0, sizeof(Gate));
    u32 handlerAddr = (u32)entry;
    gate->offsetLow = handlerAddr & 0x0ffff;
    gate->selector = selector;
    gate->dcount = 0;
    gate->attr = gateType | (privilege << 5);
    gate->offsetHigh = (handlerAddr >> 16) & 0x0ffff;
    return 1;
}

PRIVATE int protectGdtInit(Protect* protect)
{
    memset(protect->gdt, 0,  protect->gdtLen * sizeof(Descriptor));
    Dtr gptr;
    descriptorInit(protect->gdt + 0, 0, 0, 0);
    descriptorInit(protect->gdt + 1, 0, 0xfffff, DA_CR|DA_32|DA_LIMIT_4K);
    descriptorInit(protect->gdt + 2, 0, 0xfffff, DA_DRW|DA_32|DA_LIMIT_4K);
    descriptorInit(protect->gdt + 3, 0xb8000, 0xffff, DA_DRW|DA_DPL3);
    protect->selectorCode = sizeof(Descriptor) * 1;
    protect->selectorData = sizeof(Descriptor) * 2;
    protect->selectorVideo = sizeof(Descriptor) * 3;
    protect->gdtEnd = 4;
    gptr.base = (u32)(protect->gdt);
    gptr.limit = protect->gdtLen * sizeof(Descriptor) - 1;
    __asm__("lgdt %0"::"m"(gptr)); 	//加载新的gptr
    return 1;
}

PRIVATE int protectTssInit(Protect* protect)
{
    memset(protect->tss, 0, sizeof(Tss));
    protect->tss->ss0 = protect->selectorData;
    protect->tss->ioBase = sizeof(Tss);
    protect->selectorTss = sizeof(Descriptor) * protect->gdtEnd;
    Descriptor descriptor;
    descriptorInit(&descriptor, (u32)(protect->tss), sizeof(Tss) - 1, DA_386_TSS);
    protectAddDescriptor(protect, &descriptor);

	//加载tr寄存器
	asm
	(
		"xor %%eax, %%eax;"
		"mov %0, %%ax;"
		"ltr %%ax"
        ::"o"(protect->selectorTss)
        :"eax"
	);
    return 1;
}
