#ifndef OS_INTERRUPT_H_
#define OS_INTERRUPT_H_


#include "type.h"


/* * * 常量定义 * * */

/* 外部中断的中断向量号 */
#define INT_VECTOR_IRQ0 	0x20 	//IRQ0对应的中断号
#define INT_VECTOR_IRQ8 	0x28 	//IRQ8对应的中断号

/* 其他中断的中断向量号 */
#define INT_VECTOR_SYSTEM_CALL  0x80    //系统调用的中断号


/* * * * * * * 类型定义 * * * * * * * * *
 * __attribute__((packed))用于取消对齐  */
typedef struct Gate Gate;
typedef struct Protect Protect;

//中断管理
typedef struct Interrupt
{
    Protect* protect;
    Gate* idt;              //中断描述符符表
    int count;              //中断的总数(中断描述符表中的描述符个数)
    int nestingLevels;      //中断嵌套层数
    void** irqHandlerTable; //硬件中断处理函数表,表中元素为函数地址
    void** systemCallTable; //系统调用表，表中元素为函数地址
    int systemCallCount;    //系统调用的个数
}__attribute__((packed)) Interrupt;


/* * * 模块的公共接口 * * */
//初始化中断处理模块
int interruptInit(Interrupt* interrupt, Protect* protect, Gate* idtBuffer, void** irqHandlerTableBuffer,
                   void** systemCallTableBuffer, int systemCallCount);
//向硬件中断服务程序表中添加一条中断处理程序
void interruptAddIrqHandler(Interrupt* interrupt, int irq, void* handler);
//向系统调用表中添加一条系统调用
void interruptAddSystemCall(Interrupt* interrupt, int callNum, void* handler);
//关闭硬件中断
#define interruptDisableIrq() do {asm("cli");} while(0)
//开启硬件中断
#define interruptEnableIrq() do {asm("sti");} while(0)

#endif //OS_INTERRUPT_H_
