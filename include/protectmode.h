#ifndef OS_PROTECT_MODE_H_
#define OS_PROTECT_MODE_H_

#include "type.h"

/* 段描述符 */
typedef struct Descriptor
{
    u16 limitLow;
	u16 baseLow;
	u8 baseMid;
	u8 attrLow;
	u8 limitHighAttrHight;
	u8 baseHigh;
}Descriptor;

#endif /* OS_PROTECT_MODE_H_ */