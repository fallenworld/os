#ifndef OS_PORT_IO_H_
#define OS_PORT_IO_H_

#include "type.h"

/* 中断控制相关的常量定义 
 * INT_代表interrupt(中断) */

//8259A端口地址
//M代:master(主片),S:slave(从片),CTL:control
#define INT_M_CTL 		0x20 	//主片控制端口(ICW1的端口)
#define INT_M_CTLMASK 	0x21 	//主片中断屏蔽端口(除ICW1以外的端口)
#define INT_S_CTL 		0xa0 	//从片控制端口(ICW1的端口)
#define INT_S_CTLMASK 	0xa1 	//从片中断屏蔽端口(除ICW1以外的端口)

/* 端口IO的函数 */

//输出一个字节到端口
void outByte(u16 port, u8 value);

//从端口输入一个字节
u8 inByte(u16 port);

#endif //OS_PORT_IO_H_