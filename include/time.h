#ifndef OS_TIME_H_
#define OS_TIME_H_


/* 系统调用号 */
#define SYSCALL_GET_TICKS 0    //getTicks的系统调用号


/* 模块内部定义的变量 */
extern int ticks;


/* 模块内部定义的函数 */
//获取时钟中断次数
int getTicks();
//设置系统调用
void setupTimeSystemCall();
/* 该模块中所有的系统调用 */
int _syscall_getTicks();    //获取时钟中断次数


#endif //OS_TIME_H_
