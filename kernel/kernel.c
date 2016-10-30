#include "kernel.h"
#include "protectmode.h"
#include "interrupt.h"
#include "process.h"
#include "print.h"
#include "string.h"


void main()
{
	clearScreen();
	printStr("Kernel entered\n");
    //初始化保护模式模块
    protectModeInit();
    printStr("Protect mode moudle initialized\n");
    //初始化中断处理模块
    interruptInit();
    printStr("Interrupt moudle initialized\n");
    //初始化进程管理模块
    processManageInit();
    printStr("Process management moudle initialized\n");
    asm("sti");
    while(1);
}

