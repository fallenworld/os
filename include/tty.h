#ifndef OS_TTY_H_
#define OS_TTY_H_


/* * * 类型定义 * * */
//控制台(即当前显示器+键盘），多个tty共用一个控制台(显示器)，在同一个控制台上输入输出
typedef struct Console
{
    void* videoBuffer;  //控制台显存缓冲区
    void* startAddr;    //当前起始显示位置的显存地址
    int startY;         //起始显示位置的纵坐标
    int posX;           //当前光标位置的横坐标
    int posY;           //当前光标位置的纵坐标
    int height;         //控制台的最大行数(高度)
}Console;

void ttyEntry();


#endif //OS_TTY_H_
