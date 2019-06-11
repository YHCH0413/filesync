#include "mywidget.h"
#include "ui_mywidget.h"
#include <QThread>
#include <QTimer>
#include <QProgressBar>
#include <QDebug>
#include <QCompleter>
#include <QFile>
#include <QDomDocument>
#include <QMetaType>
#include <QTextBlock>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
QString xmlstr="path.xml";
mywidget::mywidget(ipdialog *id,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mywidget)
{
//    QString ipaddr;//这里的变量不能在下方的匿名函数中直接使用 必须放到头文件中才行
    QFile ipfile("ip.txt");
    if(ipfile.open(QIODevice::ReadOnly))
    {
        ipaddr=ipfile.readLine();
        ipfile.close();
    }
    //==========================================读取服务器ip
    connect(id,&ipdialog::reconnect,
            [=]
    {
        cout<<"对话框关闭";
        QFile ipf("ip.txt");
        if(ipf.open(QIODevice::ReadOnly))
        {
            ipaddr=ipf.readAll();
            cout<<ipaddr;
            ipf.close();
        }
        tsk->connectToHost(QHostAddress(ipaddr),12345);
    });
    //===========================================填写ip后再次连接
    ui->setupUi(this);
    ui->startbutton->setEnabled(false);//注意顺序 ui->setupUi(this);写完之后才能对ui内的控件进行操作
    cout<<QThread::currentThread()<<"主线程";
    tsk=new QTcpSocket(this);
    tsk->connectToHost(QHostAddress(ipaddr),12345);
    //连接到12345端口
    //*******************************************
    dx.createxml(xmlstr);
    QStringList qslist;//用于存储xml文件内的列表信息 添加到自动补全控件中
    QFile file(xmlstr);
    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        bool isok=doc.setContent(&file);
        if(isok)
        {
            qslist=dx.readelement(doc);
        }
    }
    cout<<"输出自动补全列表内容";
    cout<<qslist;
    //====================================================从xml中取数据 添加到自动补全控件中

    QCompleter *comp=new QCompleter(qslist,this);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    //实例化自动补全控件 并指定大小写不明感
    ui->source->setTextMargins(15,0,0,0);
    ui->source->setCompleter(comp);//
    ui->destination->setTextMargins(15,0,0,0);
    //ui->destination->setCompleter(comp);//服务器端 自动补全功能废弃  因为不可输入
    ui->destination->setFocusPolicy(Qt::NoFocus); //不可或取焦点（不可编辑） 服务器端不可输入
    //设定输入框的格式 并指定自动补全控件
    //====================================================实例化自动补全控件

    mtd=new mythread(id);
    thread=new QThread(this);
    mtd->moveToThread(thread);
    thread->start();//构造函数开启线程
    //====================================================文件传输独立线程

    mtds=new mythread2(tsk);
//    threads=new QThread(this);
//    mtds->moveToThread(threads);
//    threads->start();
    //====================================================实例化文件检索控件 该控件原计划运行于独立线程之下 后放弃 当前运行于主线程

    dia=new Dialog(tsk,this);//实例化文件路径选择框
    connect(dia,&Dialog::fdir,
            [=](QString str)//点击确定按键，将对话框标题的地址传给输入框，转换分隔符后，发送给服务器。
    {
        ui->statusutton->setEnabled(false);//确定之后 置灰传输状态按钮
        ui->destination->setText(str);//文本赋值进入输入框
        QString d=str;
        d.replace("\\","/");
        mtds->abpd=d;//将str进行简单的转换 并存入abpd
        tsk->write("#dirfinished#");//通知服务器 开始检索其文件结构
    });//文件路径选择框确定按键事件
    connect(dia,&Dialog::cdir,
            [=](QString dir)
    {
        tsk->write(("#create#"+dir).toUtf8().data());
    });
    connect(mtds,&mythread2::refreshdia,dia,&Dialog::refresh);//更新内容
    //===========================================================文件路径选择框相关

    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&mywidget::refrash);
    //connect(mtd,&mythread::refrashpb,this,&mywidget::refrash);
    //注意先实例化timer对象 才能写下面的connect函数 否则会报错
    //====================================计时器定期更新进度条

    connect(mtd,&mythread::getsize,this,&mywidget::starttimer);
    //========================================发送线程移动函数程序中 获取文件大小后 初始化进度条并 启动计时器

    connect(mtd,&mythread::sendover,this,&mywidget::dealover);
    //单个文件传输完毕后触发函数 删除列表上的首个元素====直接使用匿名内部类函数会出现跨线程传输问题

    connect(this,&mywidget::destroyed,this,&mywidget::dealthread);
    //connect(this,&mywidget::destroyed,mtds,&mythread2::checkedl);
    //============================================关闭按键 删除空文件 销毁次线程

    connect(this,&mywidget::starttraversal,mtds,&mythread2::traversal);
    //==========================================开始目录检索功能 dealtxtinle内触发

    connect(mtds,&mythread2::sendfileinfo,
          [=](int o)
    {
        if(o==0)
        {
            ui->treeView->setModel(mtds->modell);//setmodel和 指定父对象不重复
            ui->treeView->expandAll();//默认展开所有
        }
        else
        {
            ui->treeView_2->setModel(mtds->modelr);//setmodel和 指定父对象不重复
            ui->treeView_2->expandAll();//默认展开所有
        }
    });
    //===========================================================文件结构扫描结束后将内容展示在两侧的树结构上

    connect(mtds,&mythread2::markfinished,
            [=](QStringList s,QStringList d)
    {

        cout<<"本机即将发送的文件路径：";
        cout<<s;
        cout<<"服务器即将发送的文件路径：";
        cout<<d;
        mds=new QStringListModel(this);
        mds->setStringList(s);
        ui->sourcelist->setModel(mds);

        mdd=new QStringListModel(this);
        mdd->setStringList(d);
        ui->destlist->setModel(mdd);
        onewaylength=d.length();//用于单向传输 后删除右侧列表
    });
    //=============================================================文件比对完成后 填充中间的listview

    //ui->showdir->document()->findBlockByLineNumber(0).setVisible(false);
    //textedit.document.findbyid....setvisible
    //影藏的会被自动折叠
    //textedit也可以使用这个函数

    ui->source->installEventFilter(this);
    ui->destination->installEventFilter(this);
    //=========================================================安装过滤器

    ui->progressBar->setValue(0);
    //===================================================初始化进度条 初始值为0

    connect(mtds,&mythread2::taskover,
            [=]
    {
        ui->startbutton->setEnabled(false);//不允许开始转换
        ui->statusutton->setEnabled(true);//比对没有结果 这里手动将状态按钮调整至可用
        QMessageBox::about(NULL, "About", "二者不需同步,请重新选择源地址和目标地址。");

    });
    //===============================================比对结果为空时 不允许开始键工作
    connect(mtds,&mythread2::taskstart,
            [=]
    {
        ui->startbutton->setEnabled(true);//许开始转换
    });
    //===================================================比对结果非空 允许开始键工作
    connect(mtd,&mythread::clearextrdir,this,&mywidget::clearextrdir);
    //===============================================清空空文件夹记录表
    connect(mtd,&mythread::delelastone,
            [=]
    {
        mds->removeRow(0);
    });
    //=====================================modechange和syncover触发 删除最后一个文件名
    connect(this,&mywidget::startransfer,mtd,&mythread::getdata);
    //=======================================开始触发
    connect(ui->statusutton,&QPushButton::pressed,
            [=]
    {
        if(ui->statusutton->text()=="双向")
        {
            ui->statusutton->setText("单向");
            ui->labelright->setText("左侧：即将上传 右侧：即将删除");
            mtds->syncmode="oneway";
        }
        else {
            ui->statusutton->setText("双向");
            ui->labelright->setText("左侧：即将上传 右侧：即将下载");
            mtds->syncmode="twoway";
        }
        cout<<mtds->syncmode;
    });
    //=====================================点击状态按键 修改状态
}
mywidget::~mywidget()
{
    delete ui;
}

void mywidget::dealthread()//删除空文件夹+销毁次线程
{
    mtds->checkedl();//删除空文件
//    threads->quit();
//    threads->wait();
//    delete mtds;
    thread->quit();
    thread->wait();
    //销毁线程1
    delete mtd;
    //回收对象
}
void mywidget::refrash()//修改进度条
{

    if(mtd->tskmode==true)
    {
        ui->progressBar->setValue((mtd->sendlen+mtds->sendlen)/1024);
    }
    else
    {
        ui->progressBar->setValue((mtd->recvlen+mtds->recvlen)/1024);
    }
}
void mywidget::starttimer(qint64 a)//初始化进度条 每隔10ms执行refresh函数
{
    ui->progressBar->setValue(0);//初始化进度条 初始值为0
    ui->progressBar->setMaximum((a*2)/1024);
    ui->progressBar->setMinimum(0);
    if(!timer->isActive())
        timer->start(10);
}
void mywidget::on_sourcebutton_clicked()//目录选择键功能+更新abps
{
    QString path=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,"view file"/*,"G:\\codefactory\\qt\\filespace"*/));
    //toNativeSeparators 将路径中的分隔符转换为windows所用的分隔符
    //QFileDialog文件对话框中的getExistingDirectory函数-返回用户选择的路径
    //参数1 指定父类 参数2 标题 参数3 起始目录
    //起始目录不写 默认打开debug文件夹
    QString s=path;
    s.replace("\\","/");//获取输入框内的地址单右下划线被转义为双右下划线 toNativeSeparators无法处理双下划线
    ui->source->setText(s);//将获取的路径显示在文本框内
    mtds->abps=s;
    dealtxtinle(path);
}
void mywidget::on_desbutton_clicked()//目录选择键功能 触发目录选择框
{
//    QString path=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,"view file","G:\\codefactory\\qt\\filespace"));
//    //toNativeSeparators 将路径中的分隔符转换为windows所用的分隔符
//    //QFileDialog文件对话框中的getExistingDirectory函数-返回用户选择的路径
//    //参数1 指定父类 参数2 标题 参数3 起始目录
    if(flagfordialog==true)//首次触发 通知服务器从头开始检索
    {
        tsk->write("#getdrives#");
        flagfordialog=false;
    }
    else {//非首次触发  直接显示被影藏的窗口
        dia->show();
    }
}
bool mywidget::eventFilter(QObject *obj,QEvent *e)//针对两个输入框的事件过滤器
{
    if(e->type()==QEvent::KeyPress)
    {
        QKeyEvent *env=static_cast<QKeyEvent *>(e);
        //QEvent不包括key函数 所以需要转为keyevent
        cout<<env->key();
        if(env->key()==Qt::Key_Enter||env->key()==Qt::Key_Return)//enter小键盘 return大键盘
        {
            if(obj==ui->source)
            {
                QString path=ui->source->text();
                dealtxtinle(path);
                return 0;
            }
            else
            {
//                QString path=ui->destination->text();
//                dealtxtinle(path,ui->destination);
                //服务器端的输入功能废弃 不可直接输入
                return 1;
            }
        }
        else
        {
            return QWidget::eventFilter(obj,e);//重新分发
        }
    }
    else
    {
        return QWidget::eventFilter(obj,e);//重新分发
    }
}

void mywidget::dealtxtinle(QString path)//对文本框内的目录发起初次检索
{
    dx.addelement(xmlstr,path);
    QDir dir(path);//路径绑定dir对象
    dir.setFilter(QDir::Files|QDir::NoDotAndDotDot|QDir::Dirs);
    //筛选条件 文件 没有.和.. 路径 针对dir对象的筛选
    if(!dir.entryInfoList().isEmpty())//如果路径下不为空
    {
        mtds->list=dir.entryInfoList();//直接覆盖 不需清空
    }
    else
    {
        mtds->list.clear();//二次点击若为空 会显示之前存储的内容
    }
    //获取该路径下所有文件的QFileinfo对象（一次性获取所有信息，不需要单独取）
    emit starttraversal();//开始检索信号
}

void mywidget::dealover()//单个文件传输完毕后移除中间listview中的首个文件名
{

    //强制删除最后一个
    if(mtd->tskmode==true)
    {
        mds->removeRow(0);
        x++;
        cout<<x;
        if(x==mtds->pathlist.length())
        {
            x=0;
        }
    }
    else {
        mdd->removeRow(0);
        x++;
        cout<<x;
        if(x==mtds->dpathlist.length())
        {
            x=0;
            cout<<"传输完成";
        }
    }
}
//void mywidget::closeEvent(QCloseEvent *event)将功能放置在destory事件中实现
//{
//    mtds->checkedl();e
//    return QWidget::closeEvent(event);
//}

void mywidget::on_startbutton_clicked()//开始按键 启动数据移动函数
{
    mtd->tskmode=true;//开始前将tskmode修改
    /*connect(this,&mywidget::startransfer,mtd,&mythread::getdata);*///两次连接 会触发两次
    emit startransfer(mtds->pathlist,mtds->dpathlist,mtds->abps,mtds->abpd,mtds->dmtimelist);
    tsk->write("#transferstart#");//通过该通道向服务器传达准备传输指令 服务器一侧将进行检查
    ui->startbutton->setEnabled(false);//点击完毕之后立即置灰
    ui->statusutton->setEnabled(true);//比对之后 如果有结果 取消状态按钮的置灰
}
void mywidget::clearextrdir()//清空空文件夹路径+初始化两个输入框
{
    mtds->dint=0;
    mtds->sint=0;//只有传输成功 才会清空两个量 如果比对不需传输 那么修改一个地址后就会触发比对
    mtds->extradir.clear();
    if(mtds->syncmode=="oneway")
    {
        for(int i=0;i<onewaylength;i++)//限值在渲染列表时得到
        {
            mdd->removeRow(0);//循环删除首个
        }
    }
    ui->source->setText("请重新选择");
    ui->destination->setText("请重新选择");
    QMessageBox::about(NULL, "About", "文件同步完成,请重新选择源地址和目标地址。");
}
