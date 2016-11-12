#ifndef OS_SYSCALL_H_
#define OS_SYSCALL_H_

#include "type.h"

/* 系统调用号 */
#define SYS_CALL_GET_TICKS  0


//调用系统调用
u32 systemCall0(u32 callNum);      //0个参数
u32 systemCall1(u32 callNum, u32 arg1);    //1个参数
u32 systemCall2(u32 callNum, u32 arg1, u32 arg2);    //2个参数
u32 systemCall3(u32 callNum, u32 arg1, u32 arg2, u32 arg3);    //3个参数



#endif //OS_SYSCALL_H_
