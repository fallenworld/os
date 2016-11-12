#include "time.h"
#include "syscall.h"


#define HZ 100


int getTicks()
{
    return systemCall0(SYS_CALL_GET_TICKS);
}

void delay(int ms)
{
    int startTicks = getTicks();
    while(((getTicks() - startTicks) * 1000 / HZ) < ms)
    {}
}
