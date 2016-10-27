#ifndef OS_PRINT_H_
#define OS_PRINT_H_

#include "type.h"

//设置输出位置的显存的内存偏移
void setPrintMemOffset(u16 offset);

//清屏
void cleanScreen();

//输出一个十六进制位
u16 printHex(u8 b);

//输出一个32位数的十六进制形式
u16 printDwordHex(u32 value);
//输出一个16位数的十六进制形式
u16 printWordHex(u16 value);
//输出一个8位数的十六进制形式
u16 printByeHex(u8 value);

//输出一个字符
u16 printChar(unsigned char c);

//输出一个字符串
u16 printStr(char* str);

#endif //OS_PRINT_H_