#include "proc.h"

u8 taskStack[TASK_STACK_SIZE]; 	//任务栈

Process* initProcess(Process* process, ProcessEntry entry, u32 pid, char* name,
						Descriptor* gdt, int localDescriptorIndex)
{
	process->ldtSelector = localDescriptorIndex * sizeof(Descriptor);
	initDescriptor(gdt + localDescriptorIndex, (u32)process->ldt, LDT_LEN * sizeof(Descriptor), DA_LDT);
    process->ldt[0] = gdt[SELECTOR_FLAT_C / sizeof(Descriptor)];
	process->ldt[1] = gdt[SELECTOR_FLAT_RW / sizeof(Descriptor)];
	process->ldt[0].attrLow = DA_C | (PRIVILEGE_TASK << 5);
	process->ldt[1].attrLow = DA_C | (PRIVILEGE_TASK << 5);
	process->stackFrame.cs = (sizeof(Descriptor) * 0) | SA_LDT | SA_RPL1;
	process->stackFrame.ds = (sizeof(Descriptor) * 1) | SA_LDT | SA_RPL1;
	process->stackFrame.es = (sizeof(Descriptor) * 1) | SA_LDT | SA_RPL1;
	process->stackFrame.ss = (sizeof(Descriptor) * 1) | SA_LDT | SA_RPL1;
	process->stackFrame.fs = (sizeof(Descriptor) * 1) | SA_LDT | SA_RPL1;
	process->stackFrame.gs = (SELECTOR_FLAT_C & 0xfffc) | SA_RPL1;
	process->stackFrame.eip = (u32)entry;
    process->stackFrame.esp = (u32)taskStack + TASK_STACK_SIZE;
	process->stackFrame.eflags = PROC_EFLAGS;
    process->pid = pid;
	//process->name = 
    return process;
}
