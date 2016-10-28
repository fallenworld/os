#include "print.h"
#include "type.h"
#include "protectmode.h"
#include "string.h"
#include "kernel.h"
#include "proc.h"

Descriptor	GDT[GDT_LEN]; 	//全局描述符表
Gate 		IDT[IDT_LEN]; 	//中断描述符表
Tss 		globalTss; 		//系统全局的任务状态段
Process 	processTable[MAX_PROCESS_CNT]; 	//进程表(PCB表)


void main()
{
	clearScreen();
	printStr("Kernel entered\n");
	//设置全局描述符表
	setupGdt(GDT, GDT_LEN);
	printStr("GDT switched successfully\n");
	//设置中断描述符表
	setupIdt(IDT, IDT_LEN);
	printStr("IDT initialized successfully\n");
	//设置任务状态段
	setupTss(&globalTss, GDT, SELECTOR_TSS / sizeof(Descriptor));
	//打开硬件中断
	asm("sti");
	//启动第一个进程
	initProcess(processTable + 0, firstProcessEntry, 1, "init", GDT, SELECTOR_TSS / sizeof(Descriptor));
	runProcess(processTable + 0, &globalTss);
}

void firstProcessEntry()
{
	printStr("first Process entered\n");
	for(;;);
}
