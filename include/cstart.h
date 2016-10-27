#ifndef OS_KERNEL_H_
#define OS_KERNEL_H_


#define GDT_ENT_CNT 128 	//全局描述表中的描述符个数
#define IDT_ENT_CNT 256 	//中断描述表中的描述符个数

//程序入口
void cstart();

#endif //OS_KERNEL_H_