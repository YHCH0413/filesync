#include "ipdialog.h"
#include "ui_ipdialog.h"
#include <QFile>
#include <QDebug>
ipdialog::ipdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ipdialog)
{
    ui->setupUi(this);
    connect(this,&ipdialog::destroyed,[=]
    {
        qDebug()<<"close";
    });
}

ipdialog::~ipdialog()
{
    delete ui;
}

void ipdialog::on_pushButton_clicked()
{
    QFile file("ip.txt");
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(ui->lineEdit->text().toUtf8());
        file.close();
        emit reconnect();
        this->close();
    }
    else {
        qDebug()<<"打开失败";
    }

}
