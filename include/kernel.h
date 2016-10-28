#ifndef OS_KERNEL_H_
#define OS_KERNEL_H_

#define GDT_LEN 128 	//全局描述表的长度（表中描述符个数)
#define IDT_LEN 256 	//中断描述表的长度（表中描述符个数)

#define MAX_PROCESS_CNT 4 	//最大进程个数

//程序入口
void main();

void firstProcessEntry();

#endif //OS_KERNEL_H_