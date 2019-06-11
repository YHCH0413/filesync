#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "ui_mywidget.h"
#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QTcpSocket>
#include <QTimer>
#include "filetime.h"
#include "ipdialog.h"
class mythread : public QObject
{
    Q_OBJECT
public:
    explicit mythread(ipdialog *id,QObject *parent = nullptr);
    void transfile();
    int sendlen=0;
    int recvlen=0;//标记为易失性数据 这样能够保证实时修改
    QFileInfoList infolist;
  QString sourcefile;
 // QString distfile;
  QFile file;
  void dealsend();
  void getdata(QStringList pathl,QStringList dpathl,QString aps, QString apd,QStringList dmtlist);
  QTcpSocket *tsk;
  bool tskmode=true;//默认为发送状态 接收指令
  QTimer *timer;
  qint64 filesize=0;
  QStringList pathlist;
  QStringList dpathlist;
  QString abps;
  QString abpd;
  QStringList dmtimelist;
  bool flagforfirstdata=true;//首批数据
  int size=0;//单个文件大小
  QFile recvfile;//文件接收路径
  filetime filetime;
  QString ipaddr;
  void dealequal();
  void dealover();
  void reconnect();
  void delychange();
signals:
  void stoptimer();
  void getsize(qint64);
  void sendover();
  void refrashpb();
  void delelastone();
  void clearextrdir();
private:
    QDir *dir;
public slots:
};

#endif // MYTHREAD_H
