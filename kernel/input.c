#include "input.h"
#include "interrupt.h"
#include "macros.h"
#include "portio.h"
#include "string.h"


Input* inputInstance;


PUBLIC int inputInit(Input* input, EventQueue** inputTableBuffer, int inputTableLen)
{
    inputInstance = input;
    memset(input, 0, sizeof(Input));
    input->inputTable = inputTableBuffer;
    input->inputTableLen = inputTableLen;
    input->inputTableEnd = 0;
    return 1;
}

PUBLIC EventQueue* inputGetDevice(Input* intput, int deviceNum)
{
    return intput->inputTable[deviceNum];
}

PUBLIC int inputAddDevice(Input* input, EventQueue* eventQueue)
{
    int ret = input->inputTableEnd;
    input->inputTable[input->inputTableEnd] = eventQueue;
    input->inputTableEnd++;
    return ret;
}


PUBLIC int eventQueueInit(EventQueue* eventQueue, InputEvent* queueBuffer, int bufferSize)
{
    memset(eventQueue, 0, sizeof(EventQueue));
    eventQueue->buffer = queueBuffer;
    eventQueue->size = bufferSize;
}

PUBLIC int eventQueueEmpty(EventQueue* eventQueue)
{
    return !(eventQueue->count);
}

PUBLIC int eventQueueFull(EventQueue* eventQueue)
{
    return eventQueue->count == eventQueue->size;
}

PUBLIC void eventQueuePop(EventQueue* eventQueue)
{
    if (!eventQueueEmpty(eventQueue))
    {
        eventQueue->count--;
        eventQueue->begin = (eventQueue->begin + 1) % (eventQueue->size);
    }
}

PUBLIC InputEvent eventQueueFront(EventQueue* eventQueue)
{
    InputEvent event = {EVENT_TYPE_KEY, KEY_RESERVED, KEY_VALUE_BREAK};
    if (eventQueue->count != 0)
    {
        event = eventQueue->buffer[eventQueue->begin];
    }
    return event;
}

PUBLIC void eventQueuePush(EventQueue* eventQueue, InputEvent* event)
{
    if (eventQueueFull(eventQueue))
    {
        return;
    }
    eventQueue->count++;
    eventQueue->buffer[eventQueue->end] = (*event);
    eventQueue->end = (eventQueue->end + 1) % (eventQueue->size);
}

