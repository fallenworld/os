#ifndef OS_CLOCK_H_
#define OS_CLOCK_H_


/* 类型定义 */
typedef struct Interrupt Interrupt;
typedef struct TaskManager TaskManager;

//时钟管理模块
typedef struct Clock
{
    Interrupt* interrupt;
    TaskManager* taskManager;
    int ticks;
}Clock;


/* 模块的公共接口 */
//初始化
int clockInit(Clock* clock, Interrupt* interrupt, TaskManager* taskManager);


#endif //OS_CLOCK_H_
