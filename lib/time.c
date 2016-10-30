#include "time.h"
#include "interrupt.h"


int ticks;


int getTicks()
{
    return systemCall0(SYSCALL_GET_TICKS);
}

void setupTimeSystemCall()
{
    ticks = 0;
    addSystemCall(SYSCALL_GET_TICKS, _syscall_getTicks);
}

int _syscall_getTicks()
{
    return ticks;
}
