#include "keyboard.h"
#include "interrupt.h"
#include "portio.h"


KeyEventQueue keyEventQueue;


void keyboardHandler()
{
    if (intNestingLevels != 1)
    {
        return;
    }
    //向键盘事件队列中添加一个键盘事件
    u8 keyCode = inByte(KEYBOARD_PORT);
    InputEvent event;
    event.type = EVENT_TYPE_KEY;
    if (keyCode & 0x80)     //keyCode > 0x7f,则为松开按键
    {
        event.code = keyCode & 0x7f;    //按键码= 松开按键时传来的扫描码 & 0x7f
        event.value = KEY_VALUE_BREAK;
    }
    else
    {
        event.code = keyCode;
        event.value = KEY_VALUE_MAKE;
    }
    pushKeyEvent(event);

}

int keyEventEmpty()
{
    return keyEventQueue.count;
}

int keyEventFull()
{
    return keyEventQueue.count == KEY_EVENT_QUEUE_SIZE;
}

void popKeyEvent()
{
    if (keyEventQueue.count != 0)
    {
        keyEventQueue.count--;
        keyEventQueue.begin = (keyEventQueue.begin + 1) % KEY_EVENT_QUEUE_SIZE;
    }
}

InputEvent frontKeyEvent()
{
    InputEvent event = {EVENT_TYPE_KEY, KEY_RESERVED, KEY_VALUE_BREAK};
    if (keyEventQueue.count != 0)
    {
        event = keyEventQueue.buffer[keyEventQueue.begin];
    }
    return event;
}

void pushKeyEvent(InputEvnet event)
{
    if (KeyEventQueue.count == KEY_EVENT_QUEUE_SIZE)
    {
        return;
    }
    KeyEventQueue.count++;
    keyEventQueue.buffer[keyEventQueue.end];
    keyEventQueue.end = (keyEventQueue.end + 1) % KEY_EVENT_QUEUE_SIZE;
}







