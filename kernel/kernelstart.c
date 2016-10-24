#include "type.h"
#include "protectmode.h"

/* 描述符个数 */
#define GDT_ENT_CNT 128

/* 全局描述符表 */
Descripter GDT[GDT_ENT_CNT];

void kernelStart()
{
    
}