#include "dialogwd.h"
#include "ui_dialog.h"
#include <QDebug>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
Dialog::Dialog(QTcpSocket *tsk,QWidget *parent) :
    QDialog(parent),tsk(tsk),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //===================================================禁用双击编辑
    connect(ui->listView,&QListView::doubleClicked,
            [=]
    {
        if(len==dir.length())//如果len指针指向最后一个元素的下一个空位 即dir长度和len相等
            dir.append(ui->listView->currentIndex().data().toString());//使用append
        else if (len<dir.length())//len指向中间的某个元素
        {
            dir[len]=ui->listView->currentIndex().data().toString();//len位置上添加新值
            len++;//指向下一个被添加的位置
            for(int q=dir.length()-1;q>=len;q--)//删除dir中len到最后的所有元素
            {
                dir.removeAt(q);
            }
        }
        len=dir.length();//重新赋值（可不写）
        showdir(len);//显示路径

        int row=ui->listView->currentIndex().row();

        cout<<"选择元素的序号为";
        cout<<row;
        char* filep;
        QByteArray temp=("#dirnum#"+QString::number(row)).toUtf8();
        filep=temp.data();
        tsk->write(filep);//发送文件夹序号字符串
        cout<<"当前选择的路径为：";
        cout<<dir;
        //获取被点击的元素的序号 并发送到服务器
    });
    //===================================================双击获取行号
    ad=new adddir(this);
    connect(ad,&adddir::senddir,
           [=](QString dir)
    {
        emit cdir(dir);
    });
    //==================================================接收新建文件夹名称 并发出到mywidget处理

}
void Dialog::refresh(QStringList list, int status)//更新本控件内容和按键状态
{
    this->show();

    showdir(len);//更新窗口标题
    int b=status%2;//除2的余数
    int a=status/2%2;//除2的结果再除2的余数（0-3 两次一定够
    //转二进制
    cout<<a<<b;
    if(a)
    {
        ui->back->setEnabled(true);
        ui->createbutton->setEnabled(true);
    }
    else {
        ui->back->setEnabled(false);
        ui->createbutton->setEnabled(false);
    }
    if(b)
    {
        ui->forward->setEnabled(true);
    }
    else {
        ui->forward->setEnabled(false);
    }
    model=new QStringListModel(this);
    model->setStringList(list);
    ui->listView->setModel(model);
}
void Dialog::closeEvent(QCloseEvent *event)//重写关闭事件
{
    this->hide();//重写关闭事件 隐藏窗口
}
Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_back_clicked()
{
    if(len>0);
    len--;
    tsk->write("#back#");
}

void Dialog::on_forward_clicked()
{
    if(len<dir.length())
        len++;
    tsk->write("#forward#");
}
void Dialog::showdir(int end)//在窗口标题处显示当前路径
{
    QString str;//每次显示都遍历dir
    for(int i=0;i<end;i++)
    {
        if(str.length()==0)
        {
            str.append(dir[i].mid(0,2));//取前两位G:→盘符+冒号 其中dir是链表
        }
        else {
            str.append("/"+dir[i]);
        }
    }
    this->setWindowTitle(str);
    finaldir=str;
}

void Dialog::on_ok_clicked()
{
    emit fdir(finaldir);
    this->hide();
}


void Dialog::on_createbutton_clicked()
{
    ad->show();
}
