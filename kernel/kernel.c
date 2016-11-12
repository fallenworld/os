#include "kernel.h"
#include "protect.h"
#include "interrupt.h"
#include "task.h"
#include "clock.h"
#include "input.h"
#include "keyboard.h"

#include "tty.h"

#include "print.h"
#include "string.h"
#include "time.h"


/* 常量 */
#define GDT_LEN             128
#define INT_CNT             256
#define IRQ_CNT             16
#define SYS_CALL_CNT        32
#define TASK_TABLE_LEN      16
#define TASK_STACK_SIZE     0x1000
#define TASK_STACK_BUF_SIZE (TASK_TABLE_LEN * TASK_STACK_SIZE)
#define EVENT_QUEUE_CNT     4


/* 全局变量 */
//保护模式模块
Protect protect;
Descriptor gdt[GDT_LEN];
Tss tss;
//中断处理模块
Interrupt interrupt;
Gate idt[INT_CNT];
void* irqHandlerTable[IRQ_CNT];
void* systemCallTable[SYS_CALL_CNT];
//进程管理模块
TaskManager taskManager;
Task taskTable[TASK_TABLE_LEN];
u8 taskStackBuffer[TASK_STACK_BUF_SIZE];
//时钟模块
Clock clock;
//输入模块
Input input;
EventQueue* eventQueueTable[EVENT_QUEUE_CNT];
Keyboard keyboard;


void processA()
{
    for(;;)
    {
        //printDwordHex(getTicks());
        printChar('A');
        delay(1000);
    }
}


void processB()
{
    for(;;)
    {
        printChar('B');
        delay(1000);
    }
}


void processC()
{
    for(;;)
    {
        printChar('C');
        delay(1000);
    }
}



void main()
{
	clearScreen();
	printStr("Kernel entered\n");
    //初始化保护模式模块
    protectInit(&protect, gdt, GDT_LEN, &tss);
    printStr("Protect mode moudle initialized\n");
    //初始化中断处理模块
    interruptInit(&interrupt, &protect, idt, irqHandlerTable, systemCallTable, SYS_CALL_CNT);
    printStr("Interrupt moudle initialized\n");
    //初始化进程管理模块
    taskManagerInit(&taskManager, &protect, taskTable, TASK_TABLE_LEN, taskStackBuffer + TASK_STACK_BUF_SIZE,
                    TASK_STACK_BUF_SIZE, TASK_STACK_SIZE);
    printStr("Process management moudle initialized\n");
    //初始化时钟模块
    clockInit(&clock, &interrupt, &taskManager);
    printStr("Clock moudle initialized\n");
    //初始化输入模块
    inputInit(&input, eventQueueTable, EVENT_QUEUE_CNT);
    keyboardInit(&keyboard, &input, &interrupt);

    //taskManagerNew(&taskManager, processA, 0, "processA");
    //taskManagerNew(&taskManager, processB, 1, "processB");
    //taskManagerNew(&taskManager, processC, 2, "processC");
    taskManagerNew(&taskManager, ttyEntry, 0, "tty0");
    interruptEnableIrq();
    while(1) {}
}

