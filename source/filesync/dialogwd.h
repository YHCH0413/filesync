#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QTcpSocket>
#include "adddir.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QTcpSocket *tsk,QWidget *parent = nullptr);
    ~Dialog();
    void refresh(QStringList list,int status);//更新本空降内容和按键状态
    void showdir(int end);//在窗口标题处显示当前路径
    QStringList dir;
    QStringList list;
    QStringListModel *model;
    QTcpSocket *tsk;
    int len=0;
    QString finaldir;//存储当前路径
    adddir *ad;
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_back_clicked();

    void on_forward_clicked();

    void on_ok_clicked();

    void on_createbutton_clicked();

signals:
    void fdir(QString str);
    void cdir(QString str);
private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
