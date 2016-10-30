#include "process.h"
#include "interrupt.h"
#include "string.h"
#include "print.h"
#include "time.h"

Process* runningProcess;
Process  processTable[MAX_PROC_CNT];
u8       taskStack[PROC_STACK_SIZE * MAX_PROC_CNT];

void processManageInit()
{
    runningProcess = 0;
    addIrqHandler(0, clockInterruptHandler);
    //启动第一个进程
    initProcess(&processTable[0], gdt, processA, 0, "ProcessA");
    initProcess(&processTable[1], gdt, processB, 1, "ProcessB");
    initProcess(&processTable[2], gdt, processC, 2, "ProcessC");
    runProcess(&processTable[2]);
}

Process* initProcess(Process* process, Descriptor* gdt, ProcessEntry entry, u32 pid, char* name)
{
    memset(process, 0, sizeof(Process));
    process->ldtSelector = (LD_START_INDEX + pid) * sizeof(Descriptor);
    initDescriptor(gdt + LD_START_INDEX + pid, (u32)process->ldt, LDT_LEN * sizeof(Descriptor) - 1,
                   DA_LDT);
    process->ldt[0] = gdt[SELECTOR_FLAT_C / sizeof(Descriptor)];
    process->ldt[1] = gdt[SELECTOR_FLAT_RW / sizeof(Descriptor)];
    process->ldt[0].attrLow = DA_CR | (PRIVILEGE_TASK << 5);
    process->ldt[1].attrLow = DA_DRW | (PRIVILEGE_TASK << 5);
    process->stackFrame.cs = (sizeof(Descriptor) * 0)  | SA_LDT | SA_RPL1;
    process->stackFrame.ds = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL1;
    process->stackFrame.es = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL1;
    process->stackFrame.ss = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL1;
    process->stackFrame.fs = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL1;
    process->stackFrame.gs = (SELECTOR_VIDEO & 0xfffc) | SA_RPL1;
	process->stackFrame.eip = (u32)entry;
    process->stackFrame.esp = (u32)taskStack + PROC_STACK_SIZE * (MAX_PROC_CNT - pid);
	process->stackFrame.eflags = PROC_EFLAGS;
    process->pid = pid;
    strcpy(process->name, name);
    return process;
}

void clockInterruptHandler()
{
    ticks++;
    static int i = 0;
    if (intNestingLevels != 1)
    {
        return;
    }
    i++;
    i %= 3;
    runProcess(&processTable[i]);
}


void delay(int time)
{
    int i,j,k;
    for (k = 0; k < time; k++)
    {
        for (i = 0; i < 100; i++)
        {
            for (j = 0; j < 100; j++) {}
        }
    }
}

void processA()
{
    for(;;)
    {
        printDwordHex(getTicks());
        //printChar('A');
        delay(1000);
    }
}


void processB()
{
    for(;;)
    {
        printChar('-');
        delay(1000);
    }
}


void processC()
{
    for(;;)
    {
        printChar('=');
        delay(1000);
    }
}

