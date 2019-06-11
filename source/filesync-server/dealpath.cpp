#include "dealpath.h"
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
dealpath::dealpath()
{
    mythread=new QThread(this);
    tfthread=new transfile();
    tfthread->moveToThread(mythread);
    mythread->start();
//======================================================实例化独立文件接收和发送线程

    dx->createxml("emptydir.xml");
    bool succ=false;

    QDomDocument doc;
    QFile docfile("emptydir.xml");

    if(docfile.open(QIODevice::ReadOnly))
    {
        succ=doc.setContent(&docfile);
        docfile.close();

        cout<<"xml文件读取成功:"<<succ;
        if(succ)
        {
            extradir=dx->readelement(doc);
            if(extradir.length()>0)
                checkedl();
        }
    }
//=========================================运行时检查xml中的路径并删除 避免程序崩溃造成空文件遗留

    ts=new QTcpServer();//监听套接字实例化
    //这不是一个控件 所以无法指定父对象
    ts->listen(QHostAddress::Any,12345);//设定监听
    QObject::connect(ts,&QTcpServer::newConnection,//独立类中使用connect时需要加上QObject::
                     [=]()
    {
        //从newconnection中获取对方套接字信息 并保持
        tsk=ts->nextPendingConnection();
        //取出对方的ip和端口信息
        QString ip=tsk->peerAddress().toString();
        quint16 port=tsk->peerPort();//qunint无符号
        //构建输出格式
        QString str=QString("文件检索-连接成功 ip:%1 port:%2]").arg(ip).arg(port);
        qDebug()<<".........................................................";
        qDebug()<<"---------------------------------------------------------";
        cout<<str;//提示连接成功
        //=================================================================================
        dirlist.clear();//路径栈清空
        now=0;
        top=0;//路径检索指针
        nowdir="";//初始化当前dir //下一次检索可以直接使用 因为路径对话框没有改变

        QObject::connect(tsk,&QTcpSocket::readyRead,
                         [=]()
        {
            //            QByteArray arr=tsk->readAll();
            QString arr=QString(tsk->readAll());
            //========================================================
            QRegExp r("#sourcefileanddir#*");
            r.setPatternSyntax(QRegExp::Wildcard);
            QRegExp rx("#dirnum#*");
            rx.setPatternSyntax(QRegExp::Wildcard);
            QRegExp rxd("#create#*");
            rxd.setPatternSyntax(QRegExp::Wildcard);
            //====================================================================正则表达式对象 在下面使用
            if(r.exactMatch(arr))
            {
                cout<<"接收客户端的文件和文件夹结构";
                QRegExp rc("#sourcefileanddir#(.*)");
                if(rc.indexIn(arr)>-1)
                {
                    arr=rc.cap(1);
                }
                dir=arr.section("###",0,0);
                cout<<"收到客户端文件夹字符\n"<<dir;
                file=arr.section("###",1,1);
                cout<<"收到客户端文件字符\n"<<file;
                timestr=arr.section("###",2,2);
                cout<<"收到客户端文件最后修改时间\n"<<timestr;
                getpos();
                return;
            }
            else if (arr=="#getdrives#")//获取驱动列表
            {
                getdrives();
                return;
            }
            else if(rx.exactMatch(arr))
            {
                QRegExp rxf("#dirnum#(.*)");//选定路径的序号 送予执行下一级遍历
                if(rxf.indexIn(arr)>-1)
                {
                    int row=rxf.cap(1).toInt();
                    getdesdir(row);
                }
                return;
            }
            else if(arr=="#back#")//触发后退
            {
                empback();
                return;
            }
            else if(arr=="#forward#")//触发前进
            {
                empforward();
                return;
            }
            else if(arr=="#dirfinished#")//开始检索子文件结构 并发送
            {
                getfilelist();
                return;
            }
            else if(arr=="#transferstart#")//服务器端检查
            {
                if(tfthread->recvfilepos.isEmpty())//服务器端判断客户端文件情况
                {
                    cout<<"转为发送模式";
//                    tfthread->tskdmode=false;//文件接收完毕 自动转为发送模式
                    //tfthread->tskd->write("#modechang#");//通知客户端进入接收状态
                    //不可在其他线程调用套接字
                    emit startsend();
                    emit tfthread->startreport();
                }
                else
                {
                    cout<<"等候客户端发送文件";
                }
                return;
            }
            else if(rxd.exactMatch(arr))//创建新文件夹
            {
                QRegExp rxdd("#create#(.*)");
                if(rxdd.indexIn(arr)>-1)
                {
                    QString newdir=rxdd.cap(1);


                    QDir ndir(nowdir+"/"+newdir);
                    ndir.mkdir(nowdir+"/"+newdir);
                    getsondir(nowdir);//再次获取子文件夹 发送给客户端
                }
                return ;
            }
        });
        QObject::connect(tsk,&QTcpSocket::disconnected,
                         [=]
        {
            cout<<"客户端连接中断";
            checkedl();//短连后立刻删除
        });
        //================================断连后删除空文件夹
    });

    connect(&timer,&QTimer::timeout,
            [=]
    {
        tsk->write(("#lenthreport#"+QString::number(tfthread->sendlen)+"#"+QString::number(tfthread->recvlen)).toUtf8().data());
//        cout<<QString::number(tfthread->sendlen)+"#"+QString::number(tfthread->recvlen);
    });
    //========================================每隔特定时间 向客户端回报接收或发送进度
    connect(tfthread,&transfile::startreport,this,&dealpath::starttr);
//            [=]
//    {
//        timer.start(20);
//    });
    connect(tfthread,&transfile::stopreport,this,&dealpath::stoptr);
   /*         [=]
    {
        timer.stop();
    });*///匿名函数被认为是从其他线程启动或停止计时器
    //=======================================开始和结束计时器
    connect(tfthread,&transfile::clearextrdir,this,&dealpath::clearextrdir);
    //======================================清空空文件夹链表
    connect(this,&dealpath::startsend,tfthread,&transfile::startsend);
    //========================================上方客户端检查 如符合要求 命令本端发送
}

void dealpath::starttr()
{
    timer.start(20);
}
void dealpath::stoptr()
{
     timer.stop();
}
void dealpath::getpos()//客户端文件列表在此与服务器文件列表比对 得出客户端需要发送的文件序号和服务器需要发送的文件序号
{
    cout<<"开始比对";
    checkedl();
    tfthread->dirpath.clear();
    tfthread->filepath.clear();
    tfthread->sendfilepos.clear();
    tfthread->recvfilepos.clear();
    tfthread->mtimelsit.clear();//常驻变量 使用前清空
    tfthread->deletefilepos.clear();//常驻变量初始化

    tfthread->syncmode=dir.section("##",0,0);//同步模式提取
    int dirnum=dir.section("##",1,1).toInt();//长度
    for (int i=2;i<dirnum+2;i++)//去除串长 从1开始
    {
        tfthread->dirpath.append(dir.section("##",i,i));
    }
//====================================获取客户端dirpath list
    int filenum=file.section("##",0,0).toInt();
    for (int i=1;i<filenum+1;i++)
    {
        tfthread->filepath.append(file.section("##",i,i));
        tfthread->mtimelsit.append(timestr.section("##",i-1,i-1));//mtimelist和filepath一一对应
    }
//===================================获取客户端filepath list和对应的修改时间
    cout<<"收到的客户端文件夹列表";
    cout<<tfthread->dirpath;
    cout<<"收到的客户端文件列表";
    cout<<tfthread->filepath;

    for (int i=0;i<tfthread->dirpath.length();i++)//创建不存在的文件夹
    {
        QDir ff(tfthread->dirpath[i]);
        if(!ff.exists())
        {
            cout<<"文件创建结果："<<ff.mkdir(tfthread->dirpath[i]);
            extradir.append(tfthread->dirpath[i]);//添加到空文件夹链表中
            dx->addelement("emptydir.xml",tfthread->dirpath[i]);
        }
    }
//===================================创建与客户端文件结构对应的文件夹
    if(tfthread->syncmode=="twoway")
    {
        cout<<"双向同步";//继续执行
    }
    else if (tfthread->syncmode=="oneway")
    {
        cout<<"单向同步";
        getpos_oneway();//跳转执行单向同步规则
        return;
    }
    else {
        cout<<"syncmode error";
        system("PAUSE");
    }
    //======================================================================================================模式判定
    //获取需要传输的文件序号
    QString symbol;//分隔符
    bool fflag=true;//首次标记
    posn=0;//计数变量
    QString finalpos;
    QVector<int> tempn;//存储在服务器中已经存在的客户端文件序号
//    cout<<"a";
//    cout<<tempn[0];
//    tempn[0]=-1;
//    cout<<"b"; 未赋值 可输出 不可更改
//    for (int i=0;i<filepath.length();i++)
//    {
//        cout<<filepath[i]<<mtimelsit[i];
//    }
    for (int i=0;i<tfthread->filepath.length();i++)
    {
        cout<<"此轮比对文件"<<tfthread->filepath[i];
        QFile fi(tfthread->filepath[i]);
        if(!fi.exists())//文件在服务器中不存在 将序号写入finalpos
        {
            cout<<"以下文件在服务器中不存在 需要发送(已经转为服务器路径）";
            cout<<tfthread->filepath[i];
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            posn++;
            if(fflag==true)
            {
                symbol="";
                fflag=false;
            }
            else {
                symbol="##";
            }
            tfthread->recvfilepos.append(i);//保存 用于接收
            finalpos.append(symbol+QString::number(i));//记录服务器中不存在文件的序号
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //========================================================获取客户端需要发送的序号
        }
        else //如果存在
        {
            cout<<"该文件在服务器中存在";
//            2019#6#10#0#20#47
//            2019#6#10#0#6#39 x下方的判断方法有问题
            if(judgetime(filetime.getlastmodify(tfthread->filepath[i]),tfthread->mtimelsit[i]))//服务器端 对应文件的最后修改时间和客户端上传文件的最后修改时间比较 谁大谁新
            {

                cout<<"比对发现 下面的文件需要更新";
                cout<<tfthread->filepath[i];
                cout<<"本机文件版本"<<filetime.getlastmodify(tfthread->filepath[i])<<"客户端文件版本"<<tfthread->mtimelsit[i];
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                posn++;
                if(fflag==true)
                {
                    symbol="";
                    fflag=false;
                }
                else {
                    symbol="##";
                }
                tfthread->recvfilepos.append(i);//保存 用于接收
                finalpos.append(symbol+QString::number(i));//记录服务器中需要被覆盖的文件的序号
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                for(int j=0;j<tfthread->alllist.length();j++)//记录需要被覆盖的文件
                {
                    if(tfthread->alllist[j].isFile())//仅针对文件
                    {
                        //cout<<filepath[i]<<alllist[j].absolutePath()+alllist[j].fileName();
                        if(tfthread->filepath[i]==tfthread->alllist[j].absolutePath()+"/"+tfthread->alllist[j].fileName())
                        {
                            tempn.append(j);//拿到服务器端需要被覆盖的客户端文件路径序号（alllist列表中已经匹配的文件的序号，用于反推alllist中不匹配文件的序号
                            break;//可优化下次开始数字
                        }
                    }
                }
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            }
            else if(filetime.getlastmodify(tfthread->filepath[i])==tfthread->mtimelsit[i])//最后修改时间相同 文件不需替换
            {
                cout<<"比对发现 下面的文件在服务器中存在 且不需要更新";
                cout<<tfthread->filepath[i];
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                for(int j=0;j<tfthread->alllist.length();j++)//记录不需要移动的文件
                {
                    if(tfthread->alllist[j].isFile())//仅针对文件
                    {
                        //cout<<filepath[i]<<alllist[j].absolutePath()+alllist[j].fileName();
                        if(tfthread->filepath[i]==tfthread->alllist[j].absolutePath()+"/"+tfthread->alllist[j].fileName())
                        {
                            tempn.append(j);//拿到服务器端已经存在的客户端文件路径序号（alllist列表中已经匹配的文件的序号，用于反推alllist中不匹配文件的序号
                            break;//可优化下次开始数字
                        }
                    }
                }
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            }
        }
    }
    QString sybol;
    bool ffflag=true;
    posd=0;
    QString finaldpos;
    QString finaltime;
    int p=0;
    for(int l=0;l<tfthread->alllist.length();l++)//反推出服务器端不存在和需要反向更新的的文件路径 《《《序号》》》
    {
        cout<<"本机文件"<<tfthread->alllist[l]<<"正在比对"<<tempn<<p<<tempn[p]<<tempn.length();
        if(l==tempn[p]&&tempn.length()!=0)
        {
            cout<<"不需更新";
            p++;//tempn指针后移
        }
        else
        {
            if(tfthread->alllist[l].isFile())//必须是文件 才做记录
            {
                cout<<"当前文件序号不匹配,需要回传";
                cout<<tfthread->alllist[l];
                posd++;//计数值
                if(ffflag==true)
                {
                    sybol="";
                    ffflag=false;
                }
                else {
                    sybol="##";
                }
                tfthread->sendfilepos.append(l);//记录 供发送时使用
                finaldpos.append(sybol+QString::number(l));//添加序号
                finaltime.append(sybol+dmtimelsit[l]);
            }

        }
//        if(p==tempn.length()-1)//tempn比对完毕后结束 不再向下寻找
//        {
//            break;
//        }//这样会导致tempn最后一个元素之后的元素无法录入
        //错误的写法！！！！！！！！！！！！！！！！！！！
    }
    //========================================================获取服务器需要发送的序号
    cout<<"客户端需要发送的文件序号";
    cout<<finalpos;
    cout<<"服务器需要发送的文件序号";
    cout<<finaldpos;
    cout<<"服务器需要发送的文件序对应的最后修改时间";
    cout<<finaltime;
    if(finalpos.isEmpty())
    {
        finalpos.append("x");
    }
    if(finaldpos.isEmpty())
    {
        finaldpos.append("x");
    }
    if(finaltime.isEmpty())
    {
        finaltime.append("x");
    }
    char* filen;
    QByteArray temp=("#finalpos#"+QString::number(posn)+"##"+finalpos+"###"+QString::number(posd)+"##"+finaldpos+"###"+finaltime).toUtf8();//长度+分隔符+内容
    //发送不符合的序号
    filen=temp.data();
    tsk->write(filen);
    cout<<"发送待更新文件序号和最后修改时间";
    cout<<filen;
//    dirpath.clear();
//    filepath.clear();//新创建变量 不需清空
}
//void dealpath::sendpos()
//{
//    char* filen;
//    QByteArray temp=(QString::number(posn)+"##"+finalpos).toUtf8();//长度+分隔符+内容
//    filen=temp.data();
//    tsk->write(filen);
//}
//connect语句无法触发函数 必须使用匿名内部类
void dealpath::getpos_oneway()
{
    //获取需要传输的文件序号
    QString symbol;//分隔符
    bool fflag=true;//首次标记
    posn=0;//计数变量
    QString finalpos;
    QVector<int> tempn;//存储在服务器中已经存在的客户端文件序号
//    tempn[0]=-1;不可更改为赋值数据
    for (int i=0;i<tfthread->filepath.length();i++)
    {
        QFile fi(tfthread->filepath[i]);
        if(!fi.exists())//文件在服务器中不存在 将序号写入finalpos
        {
            cout<<"以下文件在服务器中不存在 需要发送(已经转为服务器路径）";
            cout<<tfthread->filepath[i];
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            posn++;
            if(fflag==true)
            {
                symbol="";
                fflag=false;
            }
            else {
                symbol="##";
            }
            tfthread->recvfilepos.append(i);//保存 用于接收
            finalpos.append(symbol+QString::number(i));//记录服务器中不存在文件的序号
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //========================================================获取客户端需要发送的序号
        }
        else //如果存在
        {
//            2019#6#10#0#20#47
//            2019#6#10#0#6#39 x下方的判断方法有问题
            if(filetime.getlastmodify(tfthread->filepath[i])!=tfthread->mtimelsit[i])//服务器端 对应文件的最后修改时间和客户端上传文件的最后修改时间不同
            {

                cout<<"比对发现 下面的文件需要更新";
                cout<<tfthread->filepath[i];
                cout<<"本机文件版本"<<filetime.getlastmodify(tfthread->filepath[i])<<"客户端文件版本"<<tfthread->mtimelsit[i];
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                posn++;
                if(fflag==true)
                {
                    symbol="";
                    fflag=false;
                }
                else {
                    symbol="##";
                }
                tfthread->recvfilepos.append(i);//保存 用于接收
                finalpos.append(symbol+QString::number(i));//记录服务器中需要被覆盖的文件的序号
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                for(int j=0;j<tfthread->alllist.length();j++)//记录需要被覆盖的文件
                {
                    if(tfthread->alllist[j].isFile())//仅针对文件
                    {
                        //cout<<filepath[i]<<alllist[j].absolutePath()+alllist[j].fileName();
                        if(tfthread->filepath[i]==tfthread->alllist[j].absolutePath()+"/"+tfthread->alllist[j].fileName())
                        {
                            tempn.append(j);//拿到服务器端需要被覆盖的客户端文件路径序号（alllist列表中已经匹配的文件的序号，用于反推alllist中不匹配文件的序号
                            break;//可优化下次开始数字
                        }
                    }
                }
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            }
            else//最后修改时间相同 文件不需替换
            {
                cout<<"比对发现 下面的文件在服务器中存在";
                cout<<tfthread->filepath[i];
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                for(int j=0;j<tfthread->alllist.length();j++)//记录不需要移动的文件
                {
                    if(tfthread->alllist[j].isFile())//仅针对文件
                    {
                        //cout<<filepath[i]<<alllist[j].absolutePath()+alllist[j].fileName();
                        if(tfthread->filepath[i]==tfthread->alllist[j].absolutePath()+"/"+tfthread->alllist[j].fileName())
                        {
                            tempn.append(j);//拿到服务器端已经存在的客户端文件路径序号（alllist列表中已经匹配的文件的序号，用于反推alllist中不匹配文件的序号
                            break;//可优化下次开始数字
                        }
                    }
                }
                //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            }
        }
    }

    QString sybol;
    bool ffflag=true;
    posd=0;
    QString finaldpos="";
    QString finaltime="";
    int p=0;
    for(int l=0;l<tfthread->alllist.length();l++)//反推出服务器端要删除的文件序号
    {
        if(l==tempn[p])
        {
                p++;//tempn指针后移 该值在后移中会向后溢出一位 但是QVector自动取值为0
        }
        else
        {
            if(tfthread->alllist[l].isFile())//必须是文件 才做记录
            {
                cout<<"当前文件序号不匹配,需要删除";
                cout<<tfthread->alllist[l];
                posd++;//计数值
                if(ffflag==true)
                {
                    sybol="";
                    ffflag=false;
                }
                else {
                    sybol="##";
                }
                finaldpos.append(sybol+QString::number(l));//添加序号 用作客户端标记
                tfthread->sendfilepos.append(l);//记录 供下面删除时使用
            }
            else//如果是文件夹 准备删除客户端不存在的文件夹
            {
                if(!tfthread->dirpath.isEmpty())//如果被比较的列表已经走完 则不需比较
                {
                    int i;
                    for(i=tfthread->dirpath.length()-1;i>=0;i--)
                    {
                        cout<<tfthread->dirpath[i];
                        cout<<tfthread->alllist[l].absolutePath()+"/"+tfthread->alllist[l].fileName();
                        cout<<(tfthread->dirpath[i]==(tfthread->alllist[l].absolutePath()+"/"+tfthread->alllist[l].fileName()));
                        if(tfthread->dirpath[i]==(tfthread->alllist[l].absolutePath()+"/"+tfthread->alllist[l].fileName()))
                        {
                            cout<<i;
                            tfthread->dirpath.removeAt(i);//有相同的文件 就从链表中删除 以减小后续的比较压力 也正因如此 采用倒序比较
                            break;
                        }
                    }
                    cout<<i;
                    if(i==-1) //代表无匹配
                    {
                        tfthread->deletefilepos.append(l);//记录无匹配文件夹序号
                    }
                }
                else
                {
                    cout<<"else";
                    tfthread->deletefilepos.append(l);//直接记录
                }

            }
        }
        cout<<l;
    }
    //========================================================获取服务器需要删除文件的序号


    //========================================================删除服务器端和客户端不匹配的文件
    cout<<"客户端需要发送的文件序号";
    cout<<finalpos;
    cout<<"服务器需要发送的文件序号";
    cout<<finaldpos;
    cout<<"服务器需要发送的文件序对应的最后修改时间";
    cout<<finaltime;
    if(finalpos.isEmpty())
    {
        finalpos.append("x");
    }
    if(finaldpos.isEmpty())
    {
        finaldpos.append("x");
    }
    if(finaltime.isEmpty())
    {
        finaltime.append("x");
    }
    char* filen;
    QByteArray temp=("#finalpos#"+QString::number(posn)+"##"+finalpos+"###"+QString::number(posd)+"##"+finaldpos+"###"+finaltime).toUtf8();//长度+分隔符+内容
    //发送不符合的序号
    filen=temp.data();
    tsk->write(filen);
    cout<<"发送待更新文件序号";
    cout<<filen;
}
void dealpath::getdrives()//获取驱动器列表 存入sonlist并发送 只在首次执行
{
    sonlist.clear();//每次执行前清空sonlist
    QFileInfoList drive=QDir::drives();
    QString dirs;
    bool flagff=true;//初始化flagff
    QString symbol;
    for (int i=0;i<drive.length();i++)
    {
        if(flagff==true)
        {
            symbol="";
            flagff=false;
        }
        else {
            symbol="##";
        }
        dirs.append(symbol+drive[i].absolutePath());//只发送驱动器名字 QString变量 用于后退到驱动器界面 赋值sonlist 客户端传来序号比对
        sonlist.append(drive[i].absolutePath());//存入sonlist 必须使用absolutepath filename为空
    }
    drivelist=sonlist;//更新drivelist 保存 用于后续发送
    //**********************
    char* filen;
    QByteArray temp=("#dirs#"+QString::number(getstatus())+"##"+QString::number(sonlist.length())+"##"+dirs).toUtf8();//长度+分隔符+内容
    filen=temp.data();
    //drivestr=temp.data();;//保存输出内容 char*变量只能取到最前面的#
    tsk->write(filen);//发送该目录下的子文件夹
    drivestr=QString::number(sonlist.length())+"##"+dirs;//保存驱动器对应的发送文本 bytearray变量可以拿到
    //****************************************************
    //cout<<drivestr;
    cout<<"发送驱动器列表";
    cout<<filen;
}
void dealpath::getsondir(QString fdir)//获取参数文件下的所有子目录
{

    sonlist.clear();//每次执行前清空sonlist
    QDir dir(fdir);
    //========================
    nowdir=dir.absolutePath();//保存当前路径
    //========================
    cout<<fdir<<nowdir;//对于文件夹来说 两者相同

    dir.setFilter(QDir::NoDotAndDotDot|QDir::Dirs);//仅保留路径
    QFileInfoList dflist=dir.entryInfoList();//获取所有路径列表
    bool flagff=true;//初始化
    QString dirs;
    QString symbol;//分隔符
    for(int i=0;i<dflist.length();i++)
    {
        if(flagff==true)
        {
            symbol="";
            flagff=false;
        }
        else {
            symbol="##";
        }
        sonlist.append(dflist[i].fileName());//添加到列表中 等待客户端返回序号
        dirs.append(symbol+dflist[i].fileName());//只发送文件夹名字
    }
    cout<<dirs;
    char* filen;
    QByteArray temp=("#dirs#"+QString::number(getstatus())+"##"+QString::number(sonlist.length())+"##"+dirs).toUtf8();//长度+分隔符+内容
    filen=temp.data();
    tsk->write(filen);//发送该目录下的子文件夹
    cout<<"发送子文件夹列表";
    cout<<filen;
}
void dealpath::empback()
{
    now--;
    if(now!=0)
    {
        cout<<"后退";
        getsondir(dirlist[now-1]);
    }
    else
    {
        cout<<"后退到驱动器界面";
        nowdir="";//清空nowlist 否则会出现D://F://这样的路径 无法访问
        //后退到此处不访问getsondir函数 nowdir无法更新
        QByteArray temp=("#dirs#"+QString::number(getstatus())+"##"+drivestr).toUtf8();//长度+分隔符+内容
        //注意修改状态
        tsk->write(temp.data());//发送该目录下的子文件夹
        sonlist=drivelist;
    }
}
void dealpath::empforward()
{
    cout<<"前进";
    now++;
    getsondir(dirlist[now-1]);
}
void dealpath::getdesdir(int x)//获取序号之后生成对应文件夹下新的目录
{
    cout<<"跳转目标文件序号"<<x;
    QString fdir;
    if(nowdir=="")
        fdir=sonlist[x];
    else
        fdir=nowdir+"/"+sonlist[x];
    cout<<"跳转文件路径";
    cout<<fdir;

    if(top==now)//
    {
        top++;
        now++;//指针移动
        dirlist.append(fdir);//加入dirlist
    }
    else
    {
        dirlist[now]=fdir;
        now++;
        for(int k=top-1;k>=now;k--)//删除now和top之间的元素
        {
            dirlist.removeAt(k);
        }
        top=now;
    } //存入栈内
    //放在getsondir函数之上 这样能够保证传至正确
    getsondir(fdir);
    //=========================执行检索
    cout<<"路径栈内容";
    cout<<dirlist;
}
int dealpath::getstatus()//判断前进后退按键状态
{
    if(now==top)
    {
        if(top==0)
        {
            return 0;//00不可前进 不可后退
        }
        else {
            return 2;//可后退 10
        }
    }
    else if(now<top)
    {
        if(now>=0)
        {
            return 3;//11 可前进可后退
        }
        else
        {
            return 1;//01 可前进不可后退
        }
    }
    else {
        return -10;
    }
}
void dealpath::getfilelist()//路径选择完成后触发此函数 直接使用nowdir进行检索
{
    cout<<"开始检索";
    sendstr.clear();
    if(tfthread->alllist.length()!=0)
    {
        tfthread->alllist.clear();
        dmtimelsit.clear();
    }
    firstflag=true;
    num=0;//层级计数

    QDir ndir(nowdir);
    ndir.setFilter(QDir::Files|QDir::NoDotAndDotDot|QDir::Dirs);
    QFileInfoList qflist=ndir.entryInfoList();
    if(!qflist.isEmpty())//路径下不为空
    {
        dealfile(qflist,"");
    }
    //首级文件夹检索
    while(1)
    {
        cout<<"开始判断路径是否为空";
        if(waitdirlist.isEmpty())
        {
            char* filep;
            QByteArray temp=("#desfilelist#"+QString::number(num)+"###"+sendstr).toUtf8();//标志符+层级数+遍历结果
            filep=temp.data();
            tsk->write(filep);
            cout<<"发送文件遍历结果";
            cout<<filep;
            break;//结束循环
        }
        else
        {
            QString abp=waitdirlist[0].absolutePath();//获取list首个文件夹的绝对路径
            QDir dir(abp+"/"+waitdirlist[0].fileName());//绝对路径+文件夹名的dir对象
            QString dirname=abp+"/"+waitdirlist[0].fileName();//获取本次搜索的文件夹（路径）名
            waitdirlist.removeAt(0);//移除该对象
            //指定首个文件夹为处理对象
            dir.setFilter(QDir::Files|QDir::NoDotAndDotDot|QDir::Dirs);
            //筛选条件 文件 没有.和.. 路径
            QFileInfoList qflist=dir.entryInfoList();//该文件夹下的文件列表
            if(!qflist.isEmpty())//路径下不为空
            {
                dealfile(qflist,dirname);
            }
        }
    }
}
void dealpath::dealfile(QFileInfoList dflist,QString dirname)//路径分发和可视化treeview处理
{
    num++;//层级计数+1
    QString desstr;
    QString head;
    for (int i=0;i<dflist.length();i++)
    {
        tfthread->alllist.append(dflist[i]);//将所有的文件信息添加到alllist中
        dmtimelsit.append(filetime.getlastmodify(dflist[i].absolutePath()+"/"+dflist[i].fileName()));//记录最后改动时间
        if(dflist[i].isFile())//是否为文件 是
        {
            desstr.append("##f#"+dflist[i].absolutePath()+"/"+dflist[i].fileName());//标记+路径
        }
        else
        {//否
            desstr.append("##d#"+dflist[i].absolutePath()+"/"+dflist[i].fileName());//标记+路径
        }
        //实例化实体对象
        if(dirname=="")//如果dirname为空 首级文件 添加到model
        {
            head="first.##";
        }
        else//不是首级文件
        {
            head=dirname+"##";
        }

        if(!dflist[i].isFile())//将路径添加到dirlist中 等待后续取出路径处理
        {
            waitdirlist.append(dflist[i]);
        }
    }
    QString fstr=head+QString::number(dflist.length())+desstr;
    cout<<"层级"<<num<<"发送的内容为：";
    cout<<fstr;
    if(firstflag==true)
    {
        firstflag=false;
        symbol="";
    }
    else {
        symbol="###";
    }
    sendstr.append(symbol+fstr);//添加到sendstr中
}
void dealpath::checkedl()//移除之前创建的新文件夹
{
    cout<<"清空空文件夹";
    for(int i=extradir.length()-1;i>=0;i--)//逆序删除！！！
        {
            QDir ff(extradir[i]);
            ff.rmdir(extradir[i]);
        }
    extradir.clear();
    dx->deleteall("emptydir.xml");//清空xml
}
void dealpath::clearextrdir()//删除空文件夹的链表和本地记录
{
    extradir.clear();
    dx->deleteall("emptydir.xml");//清空xml
}
bool dealpath::judgetime(QString ser,QString cli)//判断两个事件字符串关系
{
    bool res=false;
    for(int i=0;i<ser.length();i++)
    {
        if(ser[i]!=cli[i])
        {
            res=(ser[i]<cli[i]);
            if((ser[i+1]!="#")^(cli[i+1]!="#"))//异或
            {
                res=!res;
            }
            break;
        }
    }
    return res;
}
