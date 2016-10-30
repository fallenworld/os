#ifndef OS_PROTECT_MODE_H_
#define OS_PROTECT_MODE_H_

#include "type.h"


/* 常量定义 */

#define GDT_LEN 128 	//全局描述表的长度（表中描述符个数)

/* 选择符 */
#define SELECTOR_EMPTY 		0
#define SELECTOR_FLAT_C 	0x08	//代码段选择符
#define SELECTOR_FLAT_RW 	0x10	//数据段选择符
#define SELECTOR_VIDEO 		0x18 	//显存选择符
#define SELECTOR_TSS 		0x20	//TSS选择符

/* 描述符属性 
 * DA_	:Descriptor attrbute(描述符属性)
 * D    :数据段
 * C   	:代码段
 * S    :系统段
 * R    :只读
 * RW   :读写
 * A    :已访问 */
#define	DA_32			0x4000	//32 位段
#define	DA_LIMIT_4K		0x8000	//段界限粒度为 4K 字节
//特权级
#define	DA_DPL0			0x00	//DPL = 0
#define	DA_DPL1			0x20	//DPL = 1
#define	DA_DPL2			0x40	//DPL = 2
#define	DA_DPL3			0x60	//DPL = 3
//存储段描述符类型值说明
#define	DA_DR			0x90	//存在的只读数据段类型值
#define	DA_DRW			0x92	//存在的可读写数据段属性值
#define	DA_DRWA			0x93	//存在的已访问可读写数据段类型值
#define	DA_C			0x98	//存在的只执行代码段属性值
#define	DA_CR			0x9A	//存在的可执行可读代码段属性值
#define	DA_CCO			0x9C	//存在的只执行一致代码段属性值
#define	DA_CCOR			0x9E	//存在的可执行可读一致代码段属性值
//系统段描述符类型值说明
#define	DA_LDT			0x82	//局部描述符表段类型值
#define	DA_TaskGate		0x85	//任务门类型值
#define	DA_386TSS		0x89	//可用386任务状态段类型值
#define	DA_386CGate		0x8C	//386调用门类型值
#define	DA_386IGate		0x8E	//386中断门类型值

/* 选择符属性 */
//特权级
#define SA_RPL0 	0x00 	//RPL=0
#define SA_RPL1 	0x01 	//RPL=1
#define SA_RPL2 	0x02 	//RPL=2
#define SA_RPL3 	0x03 	//RPL=3
//所在的描述符表
#define SA_GDT 	0x00 	//描述符在GDT中
#define SA_LDT 	0x04 	//描述符在LDT中

/* 中断描述符特权级 */
#define	PRIVILEGE_KRNL	0 	//内核级
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3	//用户级


/*              类型定义                *
 * __attribute__((packed))用于取消对齐  */

//段描述符
typedef struct Descriptor
{
    u16 limitLow; 			//段限长0-15位
    u16 baseLow; 			//段基址0-15位
    u8  baseMid; 			//段基址16-23位
    u8  attrLow; 			//属性字段低8位
    u8  limitHighAttrHight; //低4字节:段限长16-20位，高4字节:属性字段高4位
    u8  baseHigh; 			//段基址24-31位
}__attribute__((packed)) Descriptor;

//中断描述符(门)
typedef struct Gate
{
    u16 offsetLow; 		//偏移量0-15位
    u16 selector; 		//选择符
    u8  dcount; 		//只在调用门时有效，代表需要复制的双字参数数量
    u8  attr; 			//属性字段
    u16 offsetHigh; 	//偏移量16-31位
}__attribute__((packed)) Gate;

//gptr全局描述符表寄存器
typedef struct GdtPtr
{
    u16 limit; 	//GDT的限长
    u32 base; 	//GDT的基址
}__attribute__((packed)) GdtPtr;

typedef GdtPtr IdtPtr; 	//中段描述符表寄存器

//TSS任务状态段
typedef struct Tss
{
    u32 lastLink;
    u32 esp0;
    u32 ss0;
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldtSelector;
    u32 ioOffset;
    u32 ioBase;
}__attribute__((packed)) Tss;

typedef void (*GateEntry)();  //门入口地址


/* 模块内部定义的全局变量 */
extern Descriptor   gdt[GDT_LEN]; //全局描述符表
extern Tss          tss;          //系统全局的任务状态段


/* 模块内部定义的函数 */
//初始保护模式模块
void protectModeInit();
//设置GDT(全局描述符表)
void setupGdt(Descriptor* gdt, int descriptorCount);
//设置TSS(任务状态段)
void setupTss(Tss* tss, Descriptor* gdt, int tssIndex);
//初始化段描述符
Descriptor* initDescriptor(Descriptor* descriptor, u32 base, u32 limit, u16 attr);
//初始化门的值
Gate* initGate(Gate* gate, u8 gateType, GateEntry entry, u8 privilege);


#endif //OS_PROTECT_MODE_H_
