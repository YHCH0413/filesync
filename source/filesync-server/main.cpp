//#include <QCoreApplication>
#include <QCoreApplication>
#include "dealpath.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
//dealpath dp;//不是控件 所以不用show

//BOOL WINAPI ConsoleHandler(DWORD msgType)
//{
//    /*if (msgType == CTRL_C_EVENT)
//    {

//        _ExitFlag = 1;
//        return TRUE;
//    }
//    else*/
//    if (msgType == CTRL_CLOSE_EVENT)
//    {
//        printf("Close console window!\n");
//        dp.checkedl();//服务器程序退出时删除空文件夹
//        /* Note: The system gives you very limited time to exit in this condition */
//        return TRUE;
//    }
//    return FALSE;
//}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    dealpath dp;
    //SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    return a.exec();
}
