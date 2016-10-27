#include "print.h"
#include "type.h"
#include "protectmode.h"
#include "string.h"
#include "cstart.h"

Descriptor	GDT[GDT_ENT_CNT]; //全局描述符表
Gate 		IDT[IDT_ENT_CNT]; //中断描述符表

void cstart()
{
	clearScreen();
	printStr("Kernel entered\n");
	//设置全局描述符表
	switchGdt(GDT, GDT_ENT_CNT);
	printStr("GDT switched successfully\n");
	//设置中断描述符表
	setIdt(IDT, IDT_ENT_CNT);
	printStr("IDT initialized successfully\n");
	//打开硬件中断
}