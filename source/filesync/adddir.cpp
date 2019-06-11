#include "adddir.h"
#include "ui_adddir.h"

adddir::adddir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adddir)
{
    ui->setupUi(this);
    connect(ui->ok,&QPushButton::pressed,
            [=]
    {
        QString dir=ui->lineEdit->text();
        emit senddir(dir);
        this->hide();
        ui->lineEdit->clear();//清空输入框
    });
}
void adddir::closeEvent(QCloseEvent *event)
{
    this->hide();//重写关闭事件 隐藏窗口
}
adddir::~adddir()
{
    delete ui;
}
