#ifndef OS_PROTECT_MODE_H_
#define OS_PROTECT_MODE_H_

#include "type.h"

/* 选择符 */
#define SELECTOR_EMPTY 		0
#define SELECTOR_FLAT_C 	0x08	//代码段选择符
#define SELECTOR_FLAT_RW 	0x10	//数据段选择符
#define SELECTOR_VIDEO 		0x18 	//显存选择符
#define SELECTOR_TSS 		0x20	//TSS选择符

/* 外部中断的中断向量号 */
#define INT_VECTOR_IRQ0 	0x20 	//IRQ0对应的中断号
#define INT_VECTOR_IRQ8 	0x28 	//IRQ8对应的中断号

/*            结构体定义                *
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

typedef void (*InterruptHandler)();  //中断服务程序类型


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

/* 中断向量号 */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT	0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT	0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG	0x9
#define	INT_VECTOR_INVAL_TSS	0xa
#define	INT_VECTOR_SEG_NOT		0xb
#define	INT_VECTOR_STACK_FAULT	0xc
#define	INT_VECTOR_PROTECTION	0xd
#define	INT_VECTOR_PAGE_FAULT	0xe
#define	INT_VECTOR_COPROC_ERR	0x10


//设置GDT(全局描述符表)
void setupGdt(Descriptor* gdt, int descriptorCount);

//设置IDT(中断描述符表)
void setupIdt(Gate* idt, int gateCount);

//设置TSS(任务状态段)
void setupTss(Tss* tss, Descriptor* gdt, int tssIndex);

//设置8259A
void init8259A();

//初始化段描述符
Descriptor* initDescriptor(Descriptor* descriptor, u32 base, u32 limit, u16 attr);

//初始化门的值
Gate* initGate(Gate* gate, u8 gateType, InterruptHandler handler, u8 privilege);

//异常处理函数
void exceptionHandler(int vectorNum, int errorCode, int eip, int cs, int eflag);

//硬件中断处理函数
void hardwareInterruptHandler(int code);

#endif //OS_PROTECT_MODE_H_