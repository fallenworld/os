#include "tty.h"
#include "keyboard.h"
#include "input.h"
#include "print.h"
#include "time.h"


void ttyEntry()
{
    int leftShift = 0;
    int leftCtrl = 0;
    int leftAlt = 0;
    EventQueue* keyEventQueue = keyboardGetEventQueue();
    //从键盘事件队列里读取按键事件
    InputEvent keyEvent;
    for(;;)
    {
        if (!eventQueueEmpty(keyEventQueue))
        {
            keyEvent = eventQueueFront(keyEventQueue);
            eventQueuePop(keyEventQueue);
            if (keyEvent.code == KEY_LEFTSHIFT)
            {
                leftShift = keyEvent.value;
            }
            else if (keyEvent.code == KEY_LEFTCTRL)
            {
                leftCtrl = keyEvent.value;
            }
            else if (keyEvent.code == KEY_LEFTALT)
            {
                leftAlt = keyEvent.value;
            }
            else if (keyboardIsPrintable(keyEvent.code))
            {
                if (keyEvent.value == KEY_VALUE_BREAK)
                {
                    PrintableKey printable = keyboardGetPrintable(keyEvent.code);
                    if (!leftShift)
                    {
                        printChar(printable.noShift);
                    }
                    else
                    {
                        printChar(printable.shift);
                    }
                }
            }
        }
    }
}
