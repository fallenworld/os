#ifndef OS_TASK_H_
#define OS_TASK_H_


#include "type.h"
#include "protect.h"


/* 常量定义 */
#define PROC_EFLAGS     0x1202  //进程的标志寄存器值(IF=1,IOPL=1)
#define PROC_STACK_SIZE 0x8000  //一个进程的任务栈的大小
#define PROC_MAX_CNT    16      //最大进程个数
#define LDT_LEN         2       //局部描述表的长度(表中描述符个数)


/*             类型定义                *
 * __attribute__((packed))用于取消对齐  */

//进程栈帧
typedef struct StackFrame
{
    u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
    u32 kernelEsp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 retAddr;
	u32 eip;
	u32 cs;
	u32 eflags;
	u32 esp;
	u32 ss;
}__attribute__((packed)) StackFrame;

//进程控制块
typedef struct Task
{
    StackFrame stackFrame; 		//保存寄存器状态的栈帧
    u16 selectorLdt; 			//局部描述符表的选择符
	Descriptor ldt[LDT_LEN]; 	//局部描述符表
    void (*entry)();            //进程的入口函数
	u32 pid; 					//进程id
    char name[16]; 				//进程名
}__attribute__((packed)) Task;

//进程管理者
typedef struct TaskManager
{
    Protect* protect;
    Task* running;          //当前正在运行的进程
    int count;              //系统中的进程数目
    Task* taskTable;        //进程表
    int taskTableLen;       //进程表的最大长度
    int taskTableEnd;       //进程表当前的末尾元素位置的下一位置的下标
    u8* stackBase;          //进程栈空间基址(高地址)
    int stackBufferSize;     //进程栈空间的大小
    u8* stackVailedBase;    //进程栈空间中可用地址的基址
    int taskStackSize;      //一个进程所占用的进程栈大小
}__attribute__((packed)) TaskManager;



/* * * 模块的公共接口 * * */
//初始化进程管理模块
int taskManagerInit(TaskManager* taskManager, Protect* protect, Task* taskTableBuffer, int taskTableLen,
                    u8* stackBufferBase, int stackBufferSize, int taskStackSize);
//新建一个进程
Task* taskManagerNew(TaskManager* taskManager, void(*entry)(), u32 pid, char* name);
//执行进程
void taskManagerRun(TaskManager* taskManager, Task* task);
//进程调度
void taskManagerSchedule(TaskManager* taskManager);

#endif //OS_TASK_H_
