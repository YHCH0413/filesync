#include "transfile.h"
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
#include <QThread>
transfile::transfile(QObject *parent) : QObject(parent)
{

    cout<<QThread::currentThread()<<"构造所在线程";

    //=====================================================匿名函数下写发送功能
    tsd=new QTcpServer(this);//加this时 构造在主线程 其余在次线程 没加this时 都在主线程  非常重要 否则会出现当前线程下的tsk在主线程下运行
    tsd->listen(QHostAddress::Any,12346);
    QObject::connect(tsd,&QTcpServer::newConnection,//独立类中使用connect时需要加上QObject::
                     [=]()
    {
        tskd=tsd->nextPendingConnection();
        QString ip=tskd->peerAddress().toString();
        quint16 port=tskd->peerPort();
        QString str=QString("文件传输-连接成功 ip:%1 port:%2]").arg(ip).arg(port);
        qDebug()<<".........................................................";
        qDebug()<<"---------------------------------------------------------";
        cout<<str;//提示连接成功
        flagforfirstdata=true;//初始化首次标志
        reportflag=0;
        //=================================================================================
        QObject::connect(tskd,&QTcpSocket::readyRead,
                         [=]()
        {
            if(reportflag==0)//首次触发报告信号
            {
                reportflag=1;
                emit startreport();//开启进程报告
            }
            QByteArray arr=tskd->readAll();//默认为bytearray 上面一行强制转为qstring 会导致数据大小出错 同样一段数据 QString和bytearray的体积有差别 这回导致下面的体积判断有问题
            if(tskdmode==true)//该处理仅仅区分接收数据 在此端发送首 接收指令 在此端接收时 接收数据 这样做是为了避免数据中出现和指令相同的字符 导致错乱
            {
                if(flagforfirstdata==true)//如果是首批文件
                {
                    flagforfirstdata=false;//修改值
                    size=QString(arr).toInt();//获取文件大小
                    cout<<"本次接收文件大小为";
                    cout<<size;
                    recvfile.setFileName(filepath[recvfilepos[0]]);//文件写入路径 在客户端内已经完成路径地址的转换 可以直接写入
                    cout<<"本次文件路径为：";
                    cout<<filepath[recvfilepos[0]];
                    if(recvfile.open(QIODevice::WriteOnly))
                    {
                        if(size==0)//如果文件大小为0 无法进入下面的发送流程 无法重置
                        {
                            cout<<"此文件大小为0";
                            dealequal();
                        }
                        cout<<"文件打开成功，准备写入";
                    }
                    else
                    {
                        cout<<"open failed";
                    }
                }
                else
                {
                    qint64 lenor=recvfile.write(arr);//写入文件夹
                    recvlen+=lenor;//更新客户端进度条
                    cout<<"写入单包尺寸："<<lenor;
                    cout<<"总接收尺寸"<<recvlen;
                    //                    char* temp=QString::number(lenor).toUtf8().data();
                    //                    tskd->write(temp);//告知客户端接收大小 用于更新其进度条
                    //放弃服务器实时汇报进度 粘包处理相对复杂
                    if(recvlen==size)
                    {
                        recvlen=0;//初始化文件接收大小数值 这应该在dealequal();前执行 否则该函数会调用dealover停止汇报
                        dealequal();
                    }
                }
            }
            else//接收指令模式
            {
                if(arr=="#readyrecv#")
                {
                    sendfilepos.removeAt(0);
                    emtransfile();//发送下一个文件
                }
                else if(arr=="#clientinrecv#")//客户端进入接收模式
                {
                    emtransfile();//开始发送文件 这里一定有发送的内容
                }
                else if(arr=="#syncover#")
                {
                    sendlen=0;//=================发送结束 将该值清零 避免结束后进度条停留在50%
                    cout<<"本次发送结束";
                    dealover();
                }
            }
        });
    });
    //.................................................................................................................
}
void transfile::dealequal()//处理单个文件接收完毕
{
    recvfile.close();//写入完成后关闭文件
    filetime.setlastmodify(filepath[recvfilepos[0]],mtimelsit[recvfilepos[0]]);
    recvfilepos.removeAt(0);//移除首个元素（接收完成） 先移除 再判断
    if(!recvfilepos.isEmpty())//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        flagforfirstdata=true;//初始化界定值 准备接收下一个文件新系统标志归位
        tskd->write("#readyrecv#");//告知客户端 准备接收下一个文件
    }
    else
    {
        if(!sendfilepos.isEmpty())//如果待发送文件序号列表不为空
        {
            if(syncmode=="oneway")
            {
                dealover();
            }
            else
            {
                cout<<"转为发送模式";
                tskdmode=false;//文件接收完毕 自动转为发送模式
                tskd->write("#modechang#");//通知客户端进入接收状态
            }

        }
        else
        {
           dealover();
        }
    }
}
void transfile::dealover()//处理同步完成
{
    if(syncmode=="oneway")
    {
        for(int j=sendfilepos.length()-1;j>=0;j--)
        {
            cout<<"文件"<<alllist[sendfilepos[j]].absolutePath()+"/"+alllist[sendfilepos[j]].fileName()<<"删除结果："<<QFile::remove(alllist[sendfilepos[j]].absolutePath()+"/"+alllist[sendfilepos[j]].fileName());
        }
        sendfilepos.clear();//清空该数组 避免在传输中再次发送

        for(int k=deletefilepos.length()-1;k>=0;k--)//删除文件夹
        {
            cout<<"文件夹"<<alllist[deletefilepos[k]].absolutePath()+"/"+alllist[deletefilepos[k]].fileName();
            QDir ff(alllist[deletefilepos[k]].absolutePath()+"/"+alllist[deletefilepos[k]].fileName());
            cout<<"删除结果："<<ff.rmdir(alllist[deletefilepos[k]].absolutePath()+"/"+alllist[deletefilepos[k]].fileName());
        }
        deletefilepos.clear();
    }
    cout<<"本次同步结束";
    tskd->write("#syncover#");
    emit stopreport();
    //初始化所有变量
    flagforfirstdata=true;//初始化首次标志
    reportflag=0;
    tskdmode=true;//初始化套接字模式
    emit clearextrdir();//清空空文件夹路径
}
void transfile::emtransfile()//发送文件头
{
    cout<<QThread::currentThread()<<"发送所在线程";
    if(!sendfilepos.isEmpty())
    {
        sourcefile=alllist[sendfilepos[0]].absolutePath()+"/"+alllist[sendfilepos[0]].fileName();//绝对路径
        //==============================源地址
        cout<<"本次发送文件路径为：";
        cout<<sourcefile;
        //==============================
        QFileInfo info(sourcefile);//文件信息对象和文件绑定

        sendfile.setFileName(sourcefile);//=QFile file(sourcefile)
        bool succ=sendfile.open(QIODevice::ReadOnly);//只读打开
        int filesize=info.size();//被发送文件的尺寸
        cout<<"待发送文件体积："<<filesize;
        if(succ)//file.open is success
        {
            //QString head=QString(filesize);//这样强制转换不是将数字 转为字符 而是转为\xE7\xBA\xBE格式
            QString head=QString::number(filesize);
            qint64 len=tskd->write(head.toUtf8());
            if(len>0)//说明头部信息发送成功
            {
                //为防止tcp粘包问题 需要通过定时器延时 强行分割文件头很文件本体
//                QTimer *timer=new QTimer();QObject::killTimer: Timers cannot be stopped from another thread
//                QObject::startTimer: Timers cannot be started from another thread
                timer=new QTimer();
                connect(this,&transfile::stoptimer,timer,&QTimer::stop);
                QObject::connect(timer,&QTimer::timeout,this,&transfile::dealsend);
                timer->start(20);//每隔20ms 发出一个timeout信号
            }
            else
            {
                cout<<"sending headinfo failed";
                sendfile.close();
            }
        }
        else
        {
            cout<<"read failed";
            sendfile.close();
        }
    }
    else
    {
        cout<<"本次同步结束";
    }
}
void transfile::dealsend()
{
    sendlen=0;//每一轮的初始化
    //    timer.stop();//停止计时器
    emit stoptimer();//停止计时器
    do{
        char buf[4*1024];//4k的字符数组空间用作缓冲
        qint64 localslen=sendfile.read(buf,sizeof(buf));
        //从文件中读取最大值为buf大小的数据，写入buf 返回值为本次实际读取数据大小
        //cout<<"本次发送包大小为"<<localslen;
        sendlen+=localslen;
        if(localslen==0)//len=0 发送完毕
        {
            cout<<"发送完毕";
            //sendlen=0;发送完毕就清空 速度太快 无法捕获
            break;
        }
        else//非首批数据 写入
        {
            tskd->write(buf,localslen);
        }
    }while (1);
    sendfile.close();//一个文件读取完毕后再关闭
}
void transfile::startsend()//本端检测后 客户端不需发送 本端直接发送
{
    if(syncmode=="oneway")
    {
        dealover();
    }
    else {
        tskdmode=false;//文件接收完毕 自动转为发送模式
        tskd->write("#modechang#");//通知客户端进入接收状态
    }

}
