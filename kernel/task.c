#include "task.h"
#include "string.h"
#include "print.h"


/* * * 全局变量 * * */
TaskManager* taskManagerInstance;


int taskManagerInit(TaskManager* taskManager, Protect* protect, Task* taskTableBuffer, int taskTableLen,
                    u8* stackBufferBase, int stackBufferSize, int taskStackSize)
{
    taskManagerInstance = taskManager;
    memset(taskManager, 0, sizeof(TaskManager));
    taskManager->protect = protect;
    taskManager->running = taskManager->taskTable;
    taskManager->count = 0;
    taskManager->taskTable = taskTableBuffer;
    taskManager->taskTableLen = taskTableLen;
    taskManager->taskTableEnd = 0;
    taskManager->stackBase = stackBufferBase;
    taskManager->stackBufferSize = stackBufferSize;
    taskManager->stackVailedBase = taskManager->stackBase;
    taskManager->taskStackSize = taskStackSize;
}

Task* taskManagerNew(TaskManager* taskManager, void(*entry)(), u32 pid, char* name)
{
    if ((taskManager->taskTableEnd + 1 >= taskManager->taskTableLen)
            || ((taskManager->stackBase - taskManager->stackVailedBase + taskManager->taskStackSize)
                > taskManager->stackBufferSize))
    {
        return (Task*)0;
    }
    Task* task = taskManager->taskTable + taskManager->taskTableEnd;
    memset(task, 0, sizeof(Task));
    task->entry = entry;
    task->pid = pid;
    strcpy(task->name, name);
    //初始化局部描述符表的描述符和选择符
    Descriptor ldtDescriptor;
    descriptorInit(&ldtDescriptor, (u32)(task->ldt), LDT_LEN * sizeof(Descriptor) - 1, DA_LDT);
    task->selectorLdt = protectAddDescriptor(taskManager->protect, &ldtDescriptor) * sizeof(Descriptor);
    //初始化局部描述符表
    task->ldt[0] = taskManager->protect->gdt[taskManager->protect->selectorCode / sizeof(Descriptor)];
    task->ldt[0].attrLow = DA_CR | (PRIVILEGE_USER << 5);
    task->ldt[1] = taskManager->protect->gdt[taskManager->protect->selectorData / sizeof(Descriptor)];
    task->ldt[1].attrLow = DA_DRW | (PRIVILEGE_USER << 5);
    //初始化栈帧
    task->stackFrame.cs = (sizeof(Descriptor) * 0)  | SA_LDT | SA_RPL3;
    task->stackFrame.ds = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL3;
    task->stackFrame.es = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL3;
    task->stackFrame.ss = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL3;
    task->stackFrame.fs = (sizeof(Descriptor) * 1)  | SA_LDT | SA_RPL3;
    task->stackFrame.gs = (taskManager->protect->selectorVideo & 0xfffc) | SA_RPL3;
    task->stackFrame.eip = (u32)entry;
    task->stackFrame.esp = (u32)(taskManager->stackVailedBase);
    task->stackFrame.eflags = PROC_EFLAGS;
    taskManager->stackVailedBase -= taskManager->taskStackSize;
    taskManager->taskTableEnd++;
    taskManager->count++;
}

void taskManagerRun(TaskManager* taskManager, Task* task)
{
    taskManager->running = task;
}

void taskManagerSchedule(TaskManager* taskManager)
{
    static int i = 0;
    i++;
    i %= 3;
    taskManagerRun(taskManager, taskManager->taskTable + i);
}

