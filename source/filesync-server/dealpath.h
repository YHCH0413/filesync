#ifndef DEALPATH_H
#define DEALPATH_H
#include <QTcpServer>//服务器监听套接字
#include <QTcpSocket>//通信套接字
#include <QTimer>
#include <QFileInfoList>
#include <QDir>
#include <QFile>
#include "docxml.h"
#include "filetime.h"
#include "transfile.h"
#include <QThread>
class dealpath:public QObject
{
    Q_OBJECT
public:
    explicit dealpath();
    void getpos();
    void sendpos();
    void getpos_oneway();
    void getsondir(QString fatherdir);
    void getdesdir(int x);
    void getdrives();
    void getfilelist();
    void dealfile(QFileInfoList dflist,QString dirname);
    void empback();
    void empforward();
    int getstatus();
    void checkedl();//检查删除空文件夹
    void starttr();
    void stoptr();
//
    void clearextrdir();
    bool judgetime(QString ser,QString cli);
    docxml *dx;
    QTcpServer *ts;
    QTcpSocket *tsk;

    QString dir;
    QString file;
    QString timestr;//首批接收文件夹和文件和最后修改时间字符串变量
    int posn=0;//比对结果 客户端文件计数
    int posd=0;//比对结果 服务器文件计数

    QFileInfoList waitdirlist;//文件检索中使用的文件夹变量 自清空
    QStringList dirlist;//存储访问过的路径 用于前进后退
    QString drivestr;//存储驱动器的输出字符
    QStringList drivelist;//存储驱动器的列表 后退到此后直接使用它们 不用再次遍历
    int now=0;//当前指针
    int top=0;//顶指针

    QString nowdir="";
    QStringList sonlist;//子文件夹列表
    QString sendstr;//文件检索 发送的结果
    bool firstflag=true;
    QString symbol;
    int num=0;
    QStringList extradir;//空文件夹链表

    int client=0;
    int server=0;

    filetime filetime;//对象
    QStringList dmtimelsit;

    QThread *mythread;
    transfile *tfthread;

    QTimer timer;


signals:
    void startsend();
};

#endif // DEALPATH_H
