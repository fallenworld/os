#ifndef OS_PROCESS_H_
#define OS_PROCESS_H_

#include "protectmode.h"


/* 常量定义 */
#define PROC_EFLAGS     0x1202  //进程的标志寄存器值(IF=1,IOPL=1)
#define PROC_STACK_SIZE 0x8000  //一个进程的任务栈的大小
#define LDT_LEN         2       //局部描述表的长度(表中描述符个数)
#define MAX_PROC_CNT    4       //最大进程个数
#define LD_START_INDEX  5       //第一个局部描述符在GDT中开始的下标


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
	u32 kernel_esp;
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
typedef struct Process
{
    StackFrame stackFrame; 		//保存寄存器状态的栈帧
	u16 ldtSelector; 			//局部描述符表的选择符
	Descriptor ldt[LDT_LEN]; 	//局部描述符表
	u32 pid; 					//进程id
    char name[16]; 				//进程名
}__attribute__((packed)) Process;

typedef void(*ProcessEntry)();  //进程入口函数


/* 模块内部定义的全局变量 */
extern Process* runningProcess;             //当前运行进程的pid
extern Process  processTable[MAX_PROC_CNT]; //进程表(PCB表)
extern u8       taskStack[PROC_STACK_SIZE * MAX_PROC_CNT]; //任务栈


/* 模块内部定义的函数 */
void delay(int time);
void processA();
void processB();
void processC();
//初始化进程管理模块
void processManageInit();
//初始化进程
Process* initProcess(Process* process, Descriptor* gdt, ProcessEntry entry, u32 pid, char* name);
//执行进程
#define runProcess(process) do { runningProcess = (process); } while(0)
//进程调度程序
void clockInterruptHandler();

#endif //OS_PROCESS_H_
