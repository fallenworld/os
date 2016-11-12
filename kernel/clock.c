#include "clock.h"
#include "interrupt.h"
#include "task.h"
#include "macros.h"
#include "portio.h"
#include "string.h"
#include "syscall.h"


/* 全局变量 */
Clock* clockInstance;


/* 模块内部函数 */
int clock8254Init();
int syscallGetTicks();
void clockInterruptHandler();


/* 函数定义 */

PUBLIC int clockInit(Clock* clock, Interrupt* interrupt, TaskManager* taskManager)
{
    clockInstance = clock;
    memset(clock, 0, sizeof(Clock));
    clock->interrupt = interrupt;
    clock->taskManager = taskManager;
    clock->ticks = 0;
    //设置8254
    if (!clock8254Init())
    {
        return 0;
    }
    //设置时钟中断
    interruptAddIrqHandler(interrupt, 0, clockInterruptHandler);
    //添加系统调用到系统调用表
    interruptAddSystemCall(interrupt, SYS_CALL_GET_TICKS, syscallGetTicks);
    return 1;
}

PRIVATE int clock8254Init()
{
    outByte(TIMER_MODE, RATE_GENERATOR);
    outByte(TIMER0, (u8)(TIMER_FREQ/HZ));
    outByte(TIMER0, (u8)((TIMER_FREQ/HZ) >> 8));
    return 1;
}

PRIVATE int syscallGetTicks()
{
    return clockInstance->ticks;
}

PRIVATE void clockInterruptHandler()
{
    clockInstance->ticks++;
    if (clockInstance->interrupt->nestingLevels != 1)
    {
        return;
    }
    taskManagerSchedule(clockInstance->taskManager);
}

