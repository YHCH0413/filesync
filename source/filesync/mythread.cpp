#include "mythread.h"
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
#include <QTimer>
#include <QThread>
#include <QHostAddress>

mythread::mythread(ipdialog *id,QObject *parent) : QObject(parent)/*,pathlist(pathlist),abps(abps),abpd(abpd)*/
{

    QFile ipfile("ip.txt");
    if(ipfile.open(QIODevice::ReadOnly))
    {
        ipaddr=ipfile.readLine();
        ipfile.close();
    }
    //==========================================获取服务器ip
    connect(id,&ipdialog::reconnect,this,&mythread::reconnect);
//    connect(id,&ipdialog::reconnect,
//            [=]
//    {
//        cout<<"对话框关闭";
//        QFile ipf("ip.txt");
//        if(ipf.open(QIODevice::ReadOnly))
//        {
//            ipaddr=ipf.readAll();
//            cout<<ipaddr;
//            ipf.close();
//        }
//        tsk->connectToHost(QHostAddress(ipaddr),12346);
//    });匿名函数 被认为是跨线程
    //===========================================填写ip后再次连接
    cout<<QThread::currentThread()<<"构造所在线程";
    tsk=new QTcpSocket(this);
    tsk->connectToHost(QHostAddress(ipaddr),12346);
    connect(tsk,&QTcpSocket::readyRead,
            [=]
    {
        cout<<QThread::currentThread()<<"接收所在线程";
        QByteArray arr=tsk->readAll();
        if(tskmode==true)//发送模式 接收指令
        {
            cout<<"接收命令："<<arr;
            if(arr=="#readyrecv#")//服务器准备好接收下一个文件
            {
                emit sendover();//，每传完一个文件 删除首个记录
                pathlist.removeAt(0);//先移除移动完毕的内容 再判断
                cout<<"准备发送下一个文件";
                //pathlist不可使用构造函数传值 否则无法使用removeat函数
                sendlen=0;//置零进度条
                flagforfirstdata=true;
                transfile();
            }
            else if(arr=="#modechang#")//arr=="sendover" 服务器端文件发送完毕
            {
                emit delelastone();//单方上传时无法触发
                cout<<"客户端进入接收模式";
                tskmode=false;//客户端进入接收模式
                tsk->write("#clientinrecv#");//告知服务器可以发送
            }
            else if(arr=="#syncover#")
            {
                emit delelastone();//单方上传时触发
                cout<<"本次发送结束";
                dealover();
            }
        }
        else //接收模式 接收数据
        {
            if(flagforfirstdata==true)//如果是首批文件
            {
                flagforfirstdata=false;//修改值
                cout<<arr;
                size=QString(arr).toInt();//获取文件大小
                cout<<"本次接收文件大小为："<<size<<"路径为："<<dpathlist[0].replace(0,abpd.length(),abps);//改变了值//转为本地路径//dpathlist已经被修改过
                emit getsize(size);//初始化进度条
                recvlen=0;
//                cout<<dpathlist[0];
                recvfile.setFileName(dpathlist[0]);
//                cout<<dpathlist[0];system("PAUSE");
                if(recvfile.open(QIODevice::WriteOnly))
                {
                    if(size==0)//如果文件大小为0 无法进入下面的发送流程 无法重置
                    {
                        cout<<"此文件大小为0";
                        dealequal();
                    }
                    cout<<"文件打开成功 准备写入";
                }
                else {
                    cout<<"open failed";
                }
            }
            else//tcp一次写入缓存中 然后此端才会从缓存中快速取出 不同步
            {
                qint64 lenor=recvfile.write(arr);
                recvlen+=lenor;
                //tsk->write(QString::number(lenor).toUtf8().data());//客户端自己判定文件接收尺寸
                cout<<"已接收尺寸："<<recvlen;
                if(recvlen==size)
                {
                   dealequal();
                }
            }
        }
    });
}
void mythread::dealequal()//接收单个文件成功后的后续判断函数
{
    recvfile.close();
    filetime.setlastmodify(dpathlist[0]/*.replace(0,abpd.length(),abps)*/,dmtimelist[0]);//dpathlist和dmtimelist内的文件一一对应
    //这里不需要再替换 上面已经换过了
    dmtimelist.removeAt(0);//移除首个元素
    dpathlist.removeAt(0);//移除首个元素（接收完成的）
    //666666666666666666666666666666
    emit sendover();//消除列表中的首个 这里发送信号 在对方处理之前 已经在下方将tskmode修改=====遂将tskmode=true;语句放在开始按键被点击后
    //66666666666666666666666666666666
    if(!dpathlist.isEmpty())
    {
        flagforfirstdata=true;//初始化界定值 准备接收下一个文件新系统标志归位
        tsk->write("#readyrecv#");//告知服务器 准备接收下一个文件
    }
    else {
        cout<<"客户端接收完毕，本次同步结束";
        QThread::msleep(10);
        tskmode=true;//改成发送模式 以准备接收数据
        //在debug下 程序运行正常 但是在release版本下时序被优化 这里可能会因为复位过快导致无法删除服务器列表的最后一个文件名 所以将其延后一段时间执行  遂让本线程休眠10ms
        tsk->write("#syncover#");
//        dealover();//等待服务器回话syncover 调用dealover
    }
}
void mythread::delychange()
{
    tskmode=true;//改成发送模式 以准备接收数据
    tsk->write("#syncover#");
}
void mythread::dealover()//传输结束处理函数
{
    //tskmode=true;//初始化tsk模式
    flagforfirstdata=true;
    emit clearextrdir();//清空空文件夹路径
    recvlen=sendlen=0;
    //================================
//    QMessageBox::about(NULL, "About", "文件同步完成");必须在GUI线程中创建
}
void mythread::getdata(QStringList pathl,QStringList dpathl,QString aps, QString apd,QStringList dmtlist)
{
    pathlist=pathl;//客户端需要传递的文件路径列表
    dpathlist=dpathl;//服务器需要发送的文件列表
    abps=aps;//当前源路径
    abpd=apd;//当前目标路径
    dmtimelist=dmtlist;//服务器准备发送的文件的最后修改时间 此处写入时需要修改
    transfile();//开始传输
}
void mythread::transfile()//文件头发送
{
    cout<<QThread::currentThread()<<"发送所在线程";
    if(!pathlist.isEmpty())
    {
        sourcefile=pathlist[0];
        //==============================源地址
        cout<<"本次传输文件路径为：";
        cout<<sourcefile;
        //==============================
        QFileInfo info(sourcefile);//文件信息对象和文件绑定
        filesize=info.size();//被发送文件的尺寸
        cout<<"待发送文件体积："<<filesize;
        //==============================文件信息
        emit getsize(filesize);//初始化进度条满值
        //fileinfo↑
        //QFile↓
        file.setFileName(sourcefile);//=QFile file(sourcefile)
        bool succ=file.open(QIODevice::ReadOnly);//只读打开
        sendlen=0;//初始化接收长度
        if(succ)//file.open is success
        {
            cout<<"文件成功打开 准备发送头信息";
            QString head=QString::number(filesize);//源文件地址转为目标文件地址
            qint64 len=tsk->write(head.toUtf8());
            if(len>0)//说明头部信息发送成功
            {
                timer=new QTimer();
                connect(timer,&QTimer::timeout,this,&mythread::dealsend);//指针变量
                connect(this,&mythread::stoptimer,timer,&QTimer::stop);
                //20毫秒延时 后 触发发送
                timer->start(20);
            }
            else
            {
                cout<<"sending headinfo failed";
                file.close();
            }
        }
        else
        {
            cout<<"read failed";
            file.close();
        }
    }
    else
    {
        cout<<"本机不需上传 等待服务器响应";
    }
}
void mythread::dealsend()
{
    emit stoptimer();
    //    timer->stop();
    cout<<"进入dealsend函数";
    do{
        char buf[4*1024];//4k的字符数组空间用作缓冲
        qint64 localslen=file.read(buf,sizeof(buf));
        //从文件中读取最大值为buf大小的数据，写入buf 返回值为本次实际读取数据大小
        cout<<"本次发送包大小为"<<localslen;
        sendlen+=localslen;//使用发送端数据更新进度条
        //emit refrashpb();//通过信号触发会出现信号过于密集而排队的情况
        //cout<<"发送尺寸："<<sendlen;
        if(localslen==0)//len=0 发送完毕
        {
            cout<<"发送完毕";
            break;
        }
        else//非首批数据 写入
        {
            tsk->write(buf,localslen);//不写后面的参数时 写入数据的体积会很随机
        }
    }while (1);
    cout<<"finish";

    file.close();//一个文件读取完毕后再关闭
}
void mythread::reconnect()
{
    cout<<"对话框关闭";
    QFile ipf("ip.txt");
    if(ipf.open(QIODevice::ReadOnly))
    {
        ipaddr=ipf.readAll();
        cout<<ipaddr;
        ipf.close();
    }
    tsk->connectToHost(QHostAddress(ipaddr),12346);
}
