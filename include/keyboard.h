#ifndef OS_KEYBOARD_H_
#define OS_KEYBOARD_H_

#include "type.h"
#include "input.h"

#define SCAN_CODE_BUFFER_SIZE   6

/* * * 类型定义 * * */
typedef struct EventQueue EventQueue;
typedef struct InputEvent InputEvent;
typedef struct Input Input;
typedef struct Interrupt Interrupt;

typedef struct Keyboard
{
    Input* input;               //输入管理模块
    Interrupt* interrupt;       //中断管理模块
    int driverNum;              //键盘的输入设备号
    EventQueue keyEventQueue;   //键盘输入事件队列
    u8 scanCodeBuffer[SCAN_CODE_BUFFER_SIZE];   //扫描码缓冲区
    int scanCodeInBuffer;       //扫描码缓冲区中的扫描码个数
}Keyboard;

//可打印按键的信息
typedef struct PrintableKey
{
    u8 code;        //按键码
    char noShift;   //不按shift时对应的字符
    char shift;     //按下shift时对应的字符
}PrintableKey;


/* * * 模块的公共接口 * * */
//初始化键盘处理模块
int keyboardInit(Keyboard* keyboard, Input* input, Interrupt* interrupt);
//获取到键盘事件队列
EventQueue* keyboardGetEventQueue();
//判断按键码所对应的按键是否是可打印的按键
int keyboardIsPrintable(u8 keyCode);
//获取按键码所对应的可打印按键的信息
PrintableKey keyboardGetPrintable(u8 keyCode);

#endif //OS_KEYBOARD_H_
