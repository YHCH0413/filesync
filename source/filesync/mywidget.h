#ifndef MYWIDGET_H
#define MYWIDGET_H
#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QThread>
#include <QStringListModel>
#include <QTcpSocket>
#include "mythread2.h"
#include "docxml.h"
#include "dialogwd.h"
#include "mythread.h"
#include "ipdialog.h"
namespace Ui {
class mywidget;
}

class mywidget : public QWidget
{
    Q_OBJECT

public:
    explicit mywidget(ipdialog *ip,QWidget *parent = nullptr);
    ~mywidget();
    void refrash();//修改进度条
    void dealthread();//删除空文件夹+销毁次线程
    QTimer *timer;
    void starttimer(qint64);//初始化进度条 每隔30ms执行refresh函数
    void dealtxtinle(QString path);//对文本框内的目录发起初次检索
    void dealover();//文件发送结束处理
    void clearextrdir();//清空空文件夹路径
protected:
    bool eventFilter(QObject *obj,QEvent *e);

signals:
    void startransfer(QStringList pathlist,QStringList dpathlist,QString abps,QString abpd,QStringList dmtimelist);//开始发送
    void starttraversal();//开始检索

private slots:
    void on_sourcebutton_clicked();
    void on_desbutton_clicked();

    void on_startbutton_clicked();

private:
    Ui::mywidget *ui;
    mythread2 *mtds;
    mythread *mtd;
    QThread *thread;
    QThread *threads;
    docxml dx;
    QStringListModel *mds;//source
    QStringListModel *mdd;//des
    QTcpSocket *tsk;
    Dialog *dia=nullptr;//dialogwd的类名就是Dialog
    //初始定义为空指针
    int x=0;
    int y=0;
    bool flagfordialog=true;
    int onewaylength=0;
    QString ipaddr;
};

#endif // MYWIDGET_H
