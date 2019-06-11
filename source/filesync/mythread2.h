#ifndef MYTHREAD2_H
#define MYTHREAD2_H

#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QStandardItemModel>
#include <QTcpSocket>
#include <QBuffer>
#include <QTimer>
#include <QVector>
#include "filetime.h"
class mythread2 : public QObject
{
    Q_OBJECT
public:
    explicit mythread2(QTcpSocket *tsk,QObject *parent = nullptr);
    void traversal();
    void dealfile(QFileInfoList list,QString dirname);
    //二者配合 完成本机文件结构检索 和 左侧树结构搭建
    void markfile();
    //二者配合 完成本地文件路径的分类和发送
    void checkedl();
    //删除空文件
    QFileInfoList list;
    //在mywidget中完成首级文件检索 traversal函数中直接使用
    bool firstflag=true;//modell的首次标记
    QStandardItemModel* modell;
    QStandardItemModel* modelr;

    QFileInfoList alllist;
    QList<QStandardItem*> itlist;//list需要指明使用类型
    //本机所有文件 和 item列表
    //以上两个list一一对应
    QStringList alllists;
    QList<QStandardItem*> itlists;//list需要指明使用类型
    //服务器所有文件 和 item列表

    QStandardItem *itemProject;//所有item使用的指针对象
     QFileInfoList dirlist;//文件检索中存储文件夹的链表

     QString abps;
     QString abpd;//获取source和destination路径的中间变量 （无法直接拿到mywidget的数据
     int sint=0;
     int dint=0;//源 和目标 准备状态的判定值

     QStringList pathlist;//本机准备发送的文件列表
     QStringList dpathlist;//服务器准备发送的文件列表
     QTcpSocket *tsk;//socket对象指针
     QString tempfilepath;
     QString tempdirpath;//存储在markfile中被拆分的文件夹 和文件 的文本
     QStringList pos;//markfile中记录文件在alllist中的序号
    int filenum=0;//markfile中记录文件数量
    int res=0;//parttwo中switch参数
    QStringList dirs;//存储文件路径框的下一级目录
    int statu;//存储前进 后退状态
    QStringList extradir;//存储空文件夹路径
    QStringList despath;//存储服务器端发来的文件夹路径

    filetime filetime;
    QStringList mtimelist;//本机文件最后修改时间链表
    QString temptime;
    QStringList dmtimelist;//服务器端文件最后修改时间链表

    int sendlen=0;
    int recvlen=0;//发送/接收长度

    QString syncmode="twoway";//默认双向
signals:
    void sendfileinfo(int o);//开始渲染文件结构
    void check();//开始markfile
    void refreshdia(QStringList list, int status);//更新服务器文件路径选择框
    void markfinished(QStringList pathlist,QStringList dpathlist);//视图标记完成 将准备移动的文件列表添加的listview中
    void taskover();
    void taskstart();
};

#endif // MYTHREAD2_H
