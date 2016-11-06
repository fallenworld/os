#include "interrupt.h"
#include "macros.h"
#include "protect.h"
#include "portio.h"
#include "print.h"
#include "string.h"


#define IDT_LEN             256     //中断描述表的长度（表中描述符个数)
#define IRQ_COUNT           16      //硬件中断的个数
#define SYSTEM_CALL_COUNT   32      //系统调用的个数

/* CPU预定义中断(异常)的中断向量号 */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT	0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT	0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG	0x9
#define	INT_VECTOR_INVAL_TSS	0xa
#define	INT_VECTOR_SEG_NOT		0xb
#define	INT_VECTOR_STACK_FAULT	0xc
#define	INT_VECTOR_PROTECTION	0xd
#define	INT_VECTOR_PAGE_FAULT	0xe
#define	INT_VECTOR_COPROC_ERR	0x10


/* * * 模块内部函数声明 * * */
int interrupt8259AInit();
int interruptIdtInit(Interrupt* interrupt);
int interruptSystemCallInit(Interrupt* interrupt);
void interruptExceptionHandler(int vectorNum, int errorCode, int eip, int cs, int eflag);
void clockInterruptHandler();
//预定义的内部中断(异常)服务程序
void divideError();
void singleStepException();
void nmi();
void breakpointException();
void overflow();
void boundsCheck();
void invalOpcode();
void coprNotAvailable();
void doubleFault();
void coprSegOverrun();
void invalTss();
void segmentNotPresent();
void stackException();
void generalProtection();
void pageFault();
void coprError();
//硬件中断服务程序
void irq00();
void irq01();
void irq02();
void irq03();
void irq04();
void irq05();
void irq06();
void irq07();
void irq08();
void irq09();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();
//系统调用中断
void systemCallInterrupt();


/* * * 全局变量 * * */
Interrupt* interruptInstance;


/* * * 函数定义 * * */

PUBLIC int interruptInit(Interrupt* interrupt, Protect* protect, Gate* idtBuffer, void** irqHandlerTableBuffer,
                          void** systemCallTableBuffer, int systemCallCount)
{
    interruptInstance = interrupt;
    memset(interrupt, 0, sizeof(interrupt));
    interrupt->protect = protect;
    interrupt->idt = idtBuffer;
    interrupt->irqHandlerTable = irqHandlerTableBuffer;
    interrupt->systemCallTable = systemCallTableBuffer;
    interrupt->systemCallCount = systemCallCount;
    interrupt->count = IDT_LEN;
    interrupt->nestingLevels = 0;
    interruptAddIrqHandler(interrupt, 0, clockInterruptHandler);
    if (!interrupt8259AInit())
    {
        return 0;
    }
    if (!interruptIdtInit(interrupt))
    {
        return 0;
    }
    if (!interruptSystemCallInit(interrupt))
    {
        return 0;
    }
    return 1;
}

PUBLIC void interruptAddIrqHandler(Interrupt* interrupt, int irq, void* handler)
{
    interrupt->irqHandlerTable[irq] = handler;
}

PUBLIC void interruptAddSystemCall(Interrupt* interrupt, int callNum, void* handler)
{
    interrupt->systemCallTable[callNum] = handler;
}

PRIVATE int interrupt8259AInit()
{
    outByte(INT_M_CTL, 0x11);
    outByte(INT_S_CTL, 0x11);
    outByte(INT_M_CTLMASK, INT_VECTOR_IRQ0);
    outByte(INT_S_CTLMASK, INT_VECTOR_IRQ8);
    outByte(INT_M_CTLMASK, 0x4);
    outByte(INT_S_CTLMASK, 0x2);
    outByte(INT_M_CTLMASK, 0x1);
    outByte(INT_S_CTLMASK, 0x1);
    outByte(INT_M_CTLMASK, 0xfe);
    outByte(INT_S_CTLMASK, 0xff);
    return 1;
}

PRIVATE int interruptIdtInit(Interrupt* interrupt)
{
    memset(interrupt->idt, 0, sizeof(Gate) * interrupt->count);
    //预定义的内部中断
    gateInit(interrupt->idt + INT_VECTOR_DIVIDE, 		interrupt->protect->selectorCode, DA_386_IGate, divideError, 		 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_DEBUG,         interrupt->protect->selectorCode, DA_386_IGate, singleStepException, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_NMI, 			interrupt->protect->selectorCode, DA_386_IGate, nmi, 				 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_BREAKPOINT,    interrupt->protect->selectorCode, DA_386_IGate, breakpointException, PRIVILEGE_USER);
    gateInit(interrupt->idt + INT_VECTOR_OVERFLOW,      interrupt->protect->selectorCode, DA_386_IGate, overflow,            PRIVILEGE_USER);
    gateInit(interrupt->idt + INT_VECTOR_BOUNDS, 		interrupt->protect->selectorCode, DA_386_IGate, boundsCheck,         PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_INVAL_OP,      interrupt->protect->selectorCode, DA_386_IGate, invalOpcode, 		 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_COPROC_NOT, 	interrupt->protect->selectorCode, DA_386_IGate, coprNotAvailable,  	 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_DOUBLE_FAULT,  interrupt->protect->selectorCode, DA_386_IGate, doubleFault, 		 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_COPROC_SEG, 	interrupt->protect->selectorCode, DA_386_IGate, coprSegOverrun, 	 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_INVAL_TSS,     interrupt->protect->selectorCode, DA_386_IGate, invalTss,            PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_SEG_NOT, 		interrupt->protect->selectorCode, DA_386_IGate, segmentNotPresent, 	 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_STACK_FAULT, 	interrupt->protect->selectorCode, DA_386_IGate, stackException, 	 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_PROTECTION, 	interrupt->protect->selectorCode, DA_386_IGate, generalProtection, 	 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_PAGE_FAULT, 	interrupt->protect->selectorCode, DA_386_IGate, pageFault, 			 PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_COPROC_ERR, 	interrupt->protect->selectorCode, DA_386_IGate, coprError, 			 PRIVILEGE_KRNL);
    //外部中断
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 0, interrupt->protect->selectorCode, DA_386_IGate, irq00, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 1, interrupt->protect->selectorCode, DA_386_IGate, irq01, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 2, interrupt->protect->selectorCode, DA_386_IGate, irq02, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 3, interrupt->protect->selectorCode, DA_386_IGate, irq03, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 4, interrupt->protect->selectorCode, DA_386_IGate, irq04, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 5, interrupt->protect->selectorCode, DA_386_IGate, irq05, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 6, interrupt->protect->selectorCode, DA_386_IGate, irq06, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ0 + 7, interrupt->protect->selectorCode, DA_386_IGate, irq07, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 0, interrupt->protect->selectorCode, DA_386_IGate, irq08, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 1, interrupt->protect->selectorCode, DA_386_IGate, irq09, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 2, interrupt->protect->selectorCode, DA_386_IGate, irq10, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 3, interrupt->protect->selectorCode, DA_386_IGate, irq11, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 4, interrupt->protect->selectorCode, DA_386_IGate, irq12, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 5, interrupt->protect->selectorCode, DA_386_IGate, irq13, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 6, interrupt->protect->selectorCode, DA_386_IGate, irq14, PRIVILEGE_KRNL);
    gateInit(interrupt->idt + INT_VECTOR_IRQ8 + 7, interrupt->protect->selectorCode, DA_386_IGate, irq15, PRIVILEGE_KRNL);
    //设置idtr寄存器
    Dtr idtr;
    idtr.base = (u32)(interrupt->idt);
    idtr.limit = interrupt->count * sizeof(Gate) - 1;
    asm("lidt %0"::"m"(idtr)); 	//加载idtr
    return 1;
}

PRIVATE int interruptSystemCallInit(Interrupt* interrupt)
{
    gateInit(interrupt->idt + INT_VECTOR_SYSTEM_CALL, interrupt->protect->selectorCode, DA_386_IGate,
             systemCallInterrupt, PRIVILEGE_USER);
    return 1;
}

PRIVATE void interruptExceptionHandler(int vectorNum, int errorCode, int eip, int cs, int eflag)
{
    char* errorMsg[] =
    {
        " DE Devide error",
        " # DB RESERVED",
        " --  NMI Interrupt",
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

