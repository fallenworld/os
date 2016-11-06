#include "time.h"


#define SYS_CALL_GET_TICKS 0
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
