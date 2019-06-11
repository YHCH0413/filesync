#ifndef TRANSFILE_H
#define TRANSFILE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QVector>
#include <QDir>
#include "filetime.h"
class transfile : public QObject
{
    Q_OBJECT
public:
    explicit transfile(QObject *parent = nullptr);
    QTcpServer *tsd;
    QTcpSocket *tskd;

    int sendlen=0;
    int recvlen=0;
    QFile sendfile;
    QFile recvfile;
    bool flagforfirstdata=true;//首批文件界定符
    bool tskdmode=true;//tskd的状态 默认接收  //+
    int size=0;//文件大小
    QVector<int> sendfilepos;
    QVector<int> recvfilepos;//记录序号   //+sendfilepos=new QVector<int>();实例化写法
    QVector<int> deletefilepos;
    QStringList filepath;//分别存储客户端发送的文件和文件夹  //+
    QStringList dirpath;  //+

    void dealequal();
    void emtransfile();
    void dealsend(); //负责文件发送的两个函数
    void dealover();
    void startsend();
    filetime filetime;//修改文件的最后修改时间
    QTimer *timer;//单词启动 仅用于区分文件头和文件主体
    QFileInfoList alllist;//所有文件都在列表中  //+
    QString sourcefile;//存储发送线程中使用的目标文件路径
    QStringList mtimelsit;//存储客户端发来文件对应的最后修改时间  //+
    int reportflag=0;//开启报告机制的标记
    QString syncmode="";
signals:
    void stoptimer();
    void startreport();
    void stopreport();
    void clearextrdir();
public slots:
};

#endif // TRANSFILE_H
