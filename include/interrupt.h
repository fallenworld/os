#ifndef OS_INTERRUPT_H_
#define OS_INTERRUPT_H_

#include "protectmode.h"

/* 常量定义 */

#define IDT_LEN         256     //中断描述表的长度（表中描述符个数)
#define IRQ_COUNT       16      //硬件中断的个数
#define SYS_CALL_COUNT  32      //系统调用的个数

/* 系统预定义中断(异常)的中断向量号 */
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

/* 外部中断的中断向量号 */
#define INT_VECTOR_IRQ0 	0x20 	//IRQ0对应的中断号
#define INT_VECTOR_IRQ8 	0x28 	//IRQ8对应的中断号

#define INT_VECTOR_SYSTEM_CALL  0x80    //系统调用的中断号


/* 类型定义 */
typedef void(*InterruptHandler)();    //中断服务程序类型


/* 模块内部定义的全局变量 */
extern Gate idt[IDT_LEN];                       //中断描述符表
extern int intNestingLevels;                    //中断的嵌套次数
extern InterruptHandler irqHandler[IRQ_COUNT];  //硬件中断服务程序表
extern void* sysCallTable[SYS_CALL_COUNT];             //系统调用入口表


/* 模块内部定义的函数 */
//初始化中断处理模块
void interruptInit();
//设置8259A
void init8259A();
//设置IDT(中断描述符表)
void setupIdt(Gate* idt, int gateCount);
//异常处理函数
void exceptionHandler(int vectorNum, int errorCode, int eip, int cs, int eflag);
//向硬件中断服务程序表中添加一条中断处理程序
void addIrqHandler(int irq, InterruptHandler handler);
//向系统调用表中添加一条系统调用
void addSystemCall(int callNum, void* handler);
//设置系统调用
void setupSystemCall();
//调用系统调用
u32 systemCall0(u32 callNum);      //0个参数
u32 systemCall1(u32 callNum, u32 arg1);    //1个参数
u32 systemCall2(u32 callNum, u32 arg1, u32 arg2);    //2个参数
u32 systemCall3(u32 callNum, u32 arg1, u32 arg2, u32 arg3);    //3个参数
void systemCallInterrupt(); //处理系统调用中断的中断服务程序
//预定义的内部中断(异常)服务程序
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
//硬件中断服务程序
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

#endif //OS_INTERRUPT_H_
