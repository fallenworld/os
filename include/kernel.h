#ifndef OS_KERNEL_H_
#define OS_KERNEL_H_

#include "process.h"
#include "protectmode.h"

extern u8* kernelStack;    //内核栈

//程序入口
void main();


#endif //OS_KERNEL_H_
