#ifndef OS_PROCESS_H_
#define OS_PROCESS_H_

#include "type.h"
#include "protectmode.h"

#define LDT_LEN 2 	//局部描述表的长度(表中描述符个数)

#define PROC_EFLAGS 0x1202 //进程的标志寄存器值(IF=1,IOPL=1)

#define TASK_STACK_SIZE 2048 	//任务栈的大小

typedef void(*ProcessEntry)();

/*            结构体定义                *
 * __attribute__((packed))用于取消对齐  */

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


typedef struct Process
{
    StackFrame stackFrame; 		//保存寄存器状态的栈帧
	u16 ldtSelector; 			//局部描述符表的选择符
	Descriptor ldt[LDT_LEN]; 	//局部描述符表
	u32 pid; 					//进程id
    char name[16]; 				//进程名
}__attribute__((packed)) Process;

//初始化进程
Process* initProcess(Process* process, ProcessEntry entry, u32 pid, char* name,
						Descriptor* gdt, int localDescriptorIndex);

//执行进程
void runProcess(Process* process, Tss* tss);

#endif //OS_PROCESS_H_