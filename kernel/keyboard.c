#include "keyboard.h"
#include "input.h"
#include "interrupt.h"
#include "macros.h"
#include "string.h"
#include "portio.h"
#include "print.h"

#define KEY_EVENT_QUEUE_BUFFER_SIZE 8
#define KEYBOARD_IRQ_NUM            1
#define PRINTABLE_KEY_COUNT         48

Keyboard* keyboardInstance;
InputEvent keyEventQueueBuffer[KEY_EVENT_QUEUE_BUFFER_SIZE];
PrintableKey printableKeyList[PRINTABLE_KEY_COUNT] =
{
    { KEY_1,            '1',    '!' },
    { KEY_2,            '2',    '@' },
    { KEY_3,            '3',    '#' },
    { KEY_4,            '4',    '$' },
    { KEY_5,            '5',    '%' },
    { KEY_6,            '6',    '^' },
    { KEY_7,            '7',    '&' },
    { KEY_8,            '8',    '*' },
    { KEY_9,            '9',    '(' },
    { KEY_0,            '0',    ')' },
    { KEY_MINUS,        '-',    '_' },
    { KEY_EQUAL,        '=',    '+' },
    { KEY_Q,            'q',    'Q' },
    { KEY_W,            'w',    'W' },
    { KEY_E,            'e',    'E' },
    { KEY_R,            'r',    'R' },
    { KEY_T,            't',    'T' },
    { KEY_Y,            'y',    'Y' },
    { KEY_U,            'u',    'U' },
    { KEY_I,            'i',    'I' },
    { KEY_O,            'o',    'O' },
    { KEY_P,            'p',    'P' },
    { KEY_LEFTBRACE,    '[',    '{' },
    { KEY_RIGHTBRACE,   ']',    '}' },
    { KEY_A,            'a',    'A' },
    { KEY_S,            's',    'S' },
    { KEY_D,            'd',    'D' },
    { KEY_F,            'f',    'F' },
    { KEY_G,            'g',    'G' },
    { KEY_H,            'h',    'H' },
    { KEY_J,            'j',    'J' },
    { KEY_K,            'k',    'K' },
    { KEY_L,            'l',    'L' },
    { KEY_SEMICOLON,    ';',    ':' },
    { KEY_APOSTROPHE,   '\'',   '"' },
    { KEY_GRAVE,        '`',    '~' },
    { KEY_BACKSLASH,    '\\',   '|' },
    { KEY_Z,            'z',    'Z' },
    { KEY_X,            'x',    'X' },
    { KEY_C,            'c',    'C' },
    { KEY_V,            'v',    'V' },
    { KEY_B,            'b',    'B' },
    { KEY_N,            'n',    'N' },
    { KEY_M,            'm',    'M' },
    { KEY_COMMA,        ',',    '<' },
    { KEY_DOT,          '.',    '>' },
    { KEY_SLASH,        '/',    '?' },
    { KEY_SPACE,        ' ',    ' ' }
};


void keyboardInterruptHandler();


PUBLIC int keyboardInit(Keyboard* keyboard, Input* input, Interrupt* interrupt)
{
    keyboardInstance = keyboard;
    memset(keyboard, 0, sizeof(keyboard));
    keyboard->input = input;
    keyboard->interrupt = interrupt;
    //初始化队列
    eventQueueInit(&(keyboard->keyEventQueue), keyEventQueueBuffer, KEY_EVENT_QUEUE_BUFFER_SIZE);
    //讲键盘队列添加到输入设备表
    keyboard->driverNum = inputAddDevice(input, &(keyboard->keyEventQueue));
    //设置中断处理程序
    interruptAddIrqHandler(interrupt, KEYBOARD_IRQ_NUM, keyboardInterruptHandler);
    return 1;
}

PUBLIC EventQueue* keyboardGetEventQueue()
{
    return &(keyboardInstance->keyEventQueue);
}

PUBLIC int keyboardIsPrintable(u8 keyCode)
{
    for (int i = 0; i < PRINTABLE_KEY_COUNT; i++)
    {
        if (printableKeyList[i].code == keyCode)
        {
            return 1;
        }
    }
    return 0;
}

PUBLIC PrintableKey keyboardGetPrintable(u8 keyCode)
{
    PrintableKey printable = { 0, 0, 0 };
    for (int i = 0; i < PRINTABLE_KEY_COUNT; i++)
    {
        if (printableKeyList[i].code == keyCode)
        {
            printable = printableKeyList[i];
        }
    }
    return printable;
}

PRIVATE void keyboardInterruptHandler()
{
    interruptDisableIrq();
    InputEvent event;
    event.type = EVENT_TYPE_KEY;
    //从端口获取键盘扫描码
    u8 keyCode = inByte(KEYBOARD_PORT);
    if (keyCode == 0xe0)
    {

    }
    else if(keyCode == 0xe1)
    {

    }
    else if (keyCode & 0x80)     //keyCode > 0x7f,则为松开按键
    {
        event.code = keyCode & 0x7f;    //按键码 = 松开按键时传来的扫描码 & 0x7f
        event.value = KEY_VALUE_BREAK;
    }
    else
    {
        event.code = keyCode;
        event.value = KEY_VALUE_MAKE;
    }
    //向键盘事件队列中添加一个键盘事件
    eventQueuePush(&(keyboardInstance->keyEventQueue), &event);
    interruptEnableIrq();
}
