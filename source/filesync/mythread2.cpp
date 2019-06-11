#include "mythread2.h"
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QList>

#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
mythread2::mythread2(QTcpSocket *tsk,QObject *parent) : QObject(parent),tsk(tsk)
{
    connect(this,&mythread2::check,this,&mythread2::markfile);
    //======================================================触发markfile函数
    connect(tsk,&QTcpSocket::readyRead,
            [=]
    {
        QString arr=QString(tsk->readAll());
        cout<<"输出tcp内读取到的内容";
        cout<<arr;
        //===========================================================
        QRegExp rlen("#lenthreport#*");//接收客户端回报的接收进度
        rlen.setPatternSyntax(QRegExp::Wildcard);
        if(rlen.exactMatch(arr))
        {
            QRegExp rlend("#lenthreport#(.*)");
            if(rlend.indexIn(arr)>-1)
            {
                arr=rlend.cap(1);
                recvlen=arr.section("#",0,0).toInt();
                sendlen=arr.section("#",1,1).toInt();
            }
            else
            {
                cout<<"not capture";
            }
            return;
        }
        QRegExp rx("#dirs#*");//++++++++++++++++++++++++++++++++该模式只需用*即可
        rx.setPatternSyntax(QRegExp::Wildcard);
        if(rx.exactMatch(arr))
        {
            cout<<"开始接收下一级文件目录";
            dirs.clear();
            QRegExp rxf("#dirs#(.*)");
            if(rxf.indexIn(arr)>-1)//注意 不要遗漏>-1
            {
                arr=rxf.cap(1);

                statu=arr.section("##",0,0).toInt();
                cout<<"刷新按键状态为";
                cout<<statu;
                int len=arr.section("##",1,1).toInt();
                for (int i=2;i<len+2;i++)
                {
                    dirs.append(arr.section("##",i,i));
                }
                emit refreshdia(dirs,statu);
            }
            else
            {
                cout<<"not capture";
            }
            return;
        }
        //===========================================================
        QRegExp rxb("#finalpos#*");
        rxb.setPatternSyntax(QRegExp::Wildcard);
        if(rxb.exactMatch(arr))
        {
            cout<<"接收比对后需要移动的文件序号";
            pathlist.clear();
            dpathlist.clear();
            dmtimelist.clear();
            QRegExp rxbf("#finalpos#(.*)");
            if(rxbf.indexIn(arr)>-1)
            {
                arr=rxbf.cap(1);//获取主体内容


                QString sourcedirlist=arr.section("###",0,0);//第一部分 本机待移动文件序号部分
                int len=sourcedirlist.section("##",0,0).toInt();
                //            QVector<int> num(len);使用append时不可指定长度如果指定长度为20 那么下面的append是从第21个开始
                QVector<int> num;//新声明变量 不需初始化
                cout<<"本机待发送文件序号：";
                cout<<sourcedirlist;
                for(int i=1;i<len+1;i++)
                {
                    num.append(sourcedirlist.section("##",i,i).toInt());
                }
                for(int i=0;i<num.length();i++)//标记本机一侧需要移动的文件
                {
                    modell->setData(modell->indexFromItem(itlist[pos[num[i]].toInt()]), QBrush(QColor(150, 200, 100)), Qt::BackgroundColorRole);
                    pathlist.append(alllist[pos[num[i]].toInt()].absolutePath()+"/"+alllist[pos[num[i]].toInt()].fileName());//需要传输的文件路径放入链表中
                }

                QString desdirlist=arr.section("###",1,1);//第二部分 服务器待移动文件序号部分
                if(syncmode=="twoway")
                    cout<<"服务器待发送的文件序号：";
                else {
                    cout<<"服务器要删除的文件序号：";
                }
                cout<<desdirlist;
                QVector<int> dnum;
                int lend=desdirlist.section("##",0,0).toInt();//文件序号
                for (int e=1;e<lend+1;e++)
                {
                    dnum.append(desdirlist.section("##",e,e).toInt());
                }
                if(syncmode=="twoway")
                {
                    for(int e=0;e<dnum.length();e++)//标记服务器一侧需要移动的文件
                    {
                        modelr->setData(modelr->indexFromItem(itlists[dnum[e]]), QBrush(QColor(150, 200, 100)), Qt::BackgroundColorRole);
                        dpathlist.append(alllists[dnum[e]]);//需要传输的文件路径放入链表中
                    }
                }
                else {
                    for(int e=0;e<dnum.length();e++)//单向传输时
                    {
                        modelr->setData(modelr->indexFromItem(itlists[dnum[e]]), QBrush(QColor(0, 156, 255)), Qt::BackgroundColorRole);
                        dpathlist.append(alllists[dnum[e]]);//需要传输的文件路径放入链表中
                    }
                }
                if(syncmode=="twoway")
                {
                    QString desdirtime=arr.section("###",2,2);//第三部分 服务器待移动文件对应的最后修改时间
                    for (int r=0;r<lend;r++)
                    {
                        dmtimelist.append(desdirtime.section("##",r,r));
                    }
                }
                //===================================================================
                if(pathlist.isEmpty()&&dpathlist.isEmpty())//如果两个个链表都为空 即双方都没有上传下载需求
                {
                    emit taskover();//不允许传输 开始键置灰
                }
                else
                {
                    emit taskstart();//允许开始传输 开始键可用
                    emit markfinished(pathlist,dpathlist);//将路径添加到对应list中显示
                }
            }
            else
            {
                cout<<"not capture";
            }
            return;
        }
        //===========================================================
        QRegExp rxd("#desfilelist#*");
        rxd.setPatternSyntax(QRegExp::Wildcard);
        if(rxd.exactMatch(arr))
        {
            cout<<"接收服务器端文件结构";
            despath.clear();
            alllists.clear();
            itlists.clear();//初始化链表 否则多次运行会造成堆积

            if(modelr!=nullptr)
            {
                //modelr->deleteLater();
                //deletelater删除的是指针对象 而非清空 删除后 下方实例化将受阻  此处可手动将其指向nullptr
            }
            modelr = new QStandardItemModel(this);//指定父对象
            modelr->setHorizontalHeaderLabels(QStringList()<<QString("文件目录"));
            //实例化modelr对象 并创建文件头
            QRegExp rxdf("#desfilelist#(.*)");//截取内容
            if(rxdf.indexIn(arr)>-1)
            {
                arr=rxdf.cap(1);
                int len=arr.section("###",0,0).toInt();//层级数
                for(int j=1;j<len+1;j++)
                {
                    int flag=-1;//该变量每一轮循环使用一次

                    QString onelevel=arr.section("###",j,j);//取出一个层级
                    QString father=onelevel.section("##",0,0);//取出层级内的 父级路径
                    cout<<"当前层级父级路径为：";
                    cout<<father;
                    int elenum=onelevel.section("##",1,1).toInt();//层级内的子文件数量
                    for(int p=2;p<elenum+2;p++)
                    {
                        QString signalfile=onelevel.section("##",p,p);//取出层级内的文件单元
                        QString type=signalfile.section("#",0,0);//取出单元内的文件类型属性
                        QString fpath=signalfile.section("#",1,1);//取出单元内的文件路径
                        QString fname;
                        QRegExp rt("(.*)/(.*)");
                        if(rt.indexIn(fpath)>-1)
                        {
                            fname=rt.cap(2);//取出文件路径内的filename属性 用于后期实例化视图
                        }
                        if(type=="f")//如果是文件 创建裸对象
                        {
                            itemProject=new QStandardItem(fname);
                        }
                        else//如果是路径 添加文件夹图片
                        {
                            itemProject=new QStandardItem(QIcon("1.png"),fname);
                            despath.append(fpath);//获取每一个文件夹的路径
                        }
                        if(father=="first.")//如果是最顶层 首级文件 添加到model
                        {
                            modelr->appendRow(itemProject);
                        }
                        else//不是首级文件
                        {
                            if(flag==-1)//寻找对应的父itemProject序号
                            {
                                for(int l=0;l<alllists.length();l++)
                                {
                                    if(alllists[l]==father)
                                    {
                                        flag=l;//拿到flag值
                                        break;
                                    }
                                }
                            }
                            if(flag!=-1)//将本次itemProject添加到找到的父级之下
                            {
                                itlists[flag]->appendRow(itemProject);
                            }
                            else
                            {
                                cout<<"fetal error";
                            }
                        }
                        alllists.append(fpath);//添加到参照链表 详细路径 为了避免重名
                        itlists.append(itemProject);//添加到itemlist中
                    }
                }
            }
            emit sendfileinfo(1);//视图构建完成 触发显示

            if(dint==0)
                dint+=1;
            if(sint+dint==2)//判定数据源 和 数据目标是否齐全
            {
                cout<<"双方";
                emit check();//每次结束都执行检查
            }
            else
            {
                cout<<"单方";
            }//检查是否满足markfile要求
        }
        else
        {
            cout<<"not capture";
        }
        //===========================================================
    });
}


void mythread2::traversal()//本机文件检索函数
{
    if(alllist.length()!=0)
    {
        alllist.clear();
        itlist.clear();
        //modell->clear();
        mtimelist.clear();
    }//alllist最后存储的结果是目标文件夹中结构 如果alllist不为空 则代表源文件重新选择 源文件选择完毕之后alllist会自动清空 所以这一定是目标文件重新选择
    if(firstflag==true)
    {
        //        if(modell!=nullptr)
        //        {
        //            cout<<"删除上次创建的对象";
        //            //modell->deleteLater();
        //            //无法删除！！！！！！！！！！！！！！！！！！！！！！！！！
        //        }用clear代替
        modell = new QStandardItemModel(this);//指定父对象
        modell->setHorizontalHeaderLabels(QStringList()<<QString("文件目录"));
        firstflag=false;
        cout<<"firstflag=true，创建树视图头标题";
    }
    //设定视图的文件头 firstflag能够自洽
    dealfile(list,"");//分发文件的路径 第一级目录
    while(1)
    {
        cout<<"开始判断是否还有文件夹没有检索";
        if(dirlist.isEmpty())//如果列表已经空了
        {
            emit sendfileinfo(0);//qlist用于发送左侧内容 着色前渲染
            //清空qlist 在前面处理完成之后
            firstflag=true;//结束之后firstflag复位
            //=======================================判定是否执行markfile
            if(sint==0)
                sint+=1;
            if(sint+dint==2)//判定是否执行markfile函数
            {
                cout<<"双方";
                emit check();//每次结束都执行检查
            }
            else
            {
                cout<<"单方";
            }
            //=======================================
            break;//结束循环
        }
        else
        {
            QString abp=dirlist[0].absolutePath()+"/"+dirlist[0].fileName();//获取list首个文件夹的绝对路径+文件名 绝对路径之道上一级
            QDir dir(abp);//绝对路径+文件夹名的dir对象
            QString dirname=abp;//获取本次搜索的文件夹（路径）名 作为父文件夹路径
            //            cout<<dirname;
            //            cout<<abp;
            //            return;//path只到上一级
            dirlist.removeAt(0);//移除该对象
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
void mythread2::dealfile(QFileInfoList dflist,QString dirname)//路径分发和可视化treeview处理
{
    cout<<"本轮检索文件列表";
    cout<<dflist;
    int flag=-1;
    for (int i=0;i<dflist.length();i++)
    {
        if(dflist[i].isFile())//是否为文件
        {//是
            itemProject=new QStandardItem(dflist[i].fileName());
        }
        else {//否
            itemProject=new QStandardItem(QIcon("1.png"),dflist[i].fileName());//添加图标
        }
        //实例化实体对象

        if(dirname=="")//如果dirname为空 首级文件 添加到model
        {
            modell->appendRow(itemProject);
        }
        else//不是首级文件
        {
            if(flag==-1)//寻找对应的父itemProject序号
            {
                for(int l=0;l<alllist.length();l++)
                {
                    if(alllist[l]==dirname)//QFileinfo(G:/codefactory/qt/filespace/old)==G:/codefactory/qt/filespace/old 也可以提取abp和name进行比较
                    {
                        //cout<<alllist[l].absolutePath()+"/"+alllist[l].fileName()<<dirname;
                        flag=l;//拿到flag值
                        break;
                    }
                }
            }
            if(flag!=-1)//将本次itemProject添加到找到的父级之下
            {
                itlist[flag]->appendRow(itemProject);
            }
            else {
                cout<<"fetal error";
                system("PAUSE");
            }
        }
        alllist.append(dflist[i]);//添加到参照链表 详细路径 为了避免重名
        itlist.append(itemProject);//添加到itemlist中
        mtimelist.append(filetime.getlastmodify(dflist[i].absolutePath()+"/"+dflist[i].fileName()));//记录最后修改时间

        if(!dflist[i].isFile())//如果不是文件 添加到链表中 在下一轮中检索
        {
            dirlist.append(dflist[i]);
        }
    }
}
void mythread2::markfile()//分别发送本路径下的文件 和 文件夹 到服务器端比对
{
    cout<<"markfile开始";
    checkedl();//运行检查函数

    filenum=0;//文件数量初始化
    tempfilepath.clear();
    tempdirpath.clear();
    temptime.clear();
    pos.clear();

    bool flagf=true;
    bool flagd=true;

    for (int i=0;i<alllist.length();i++)
    {
        if(alllist[i].isFile())//是文件
        {
            filenum++;//记录文件数量
            QString symbol;
            if(flagf)
            {
                symbol="";
                flagf=false;//首次标记置否
            }
            else
            {
                symbol="##";
            }
            tempfilepath.append(symbol+QString(alllist[i].absolutePath()+"/"+alllist[i].fileName()).replace(0,abps.length(),abpd));
            //发送存储的目标路径
            pos.append(QString::number(i));//记录文件在alllist中的序号
            temptime.append(symbol+mtimelist[i]);//最后修改时间字符
        }
        else
        {
            QString symbol;
            if(flagd)
            {
                symbol="";
                flagd=false;//首次标记置否
            }
            else
            {
                symbol="##";
            }
            tempdirpath.append(symbol+QString(alllist[i].absolutePath()+"/"+alllist[i].fileName()).replace(0,abps.length(),abpd));
            //在客户端一方已经将地址进行了转换 到 服务器一端可以直接使用
            //            tempdirpath.append(QString(symbol+alllists[i].absolutePath()+"/"+alllists[i].fileName()).replace(0,abps.length(),abpd));
            //下面的写法会将##解析为ce

            //目标文件夹文件夹路径
        }
    }
    if(tempdirpath.isEmpty())
    {
        tempfilepath.append("x");
    }
    if(tempfilepath.isEmpty())
    {
        tempfilepath.append("x");
    }
    if(temptime.isEmpty())
    {
        temptime.append("x");
    }//保证字符串的格式
    cout<<"所有文件在alllist中的序号为：";
    cout<<pos;
    QByteArray temp=("#sourcefileanddir#"+syncmode+"##"+QString::number(alllist.length()-filenum)+"##"+tempdirpath+"###"+QString::number(filenum)+"##"+tempfilepath+"###"+temptime).toUtf8();
    tsk->write(temp);

    //创建与服务器端对应的文件夹
    //extradir.clear();//添加之前清空
    if(syncmode=="twoway")//单向同步时 不需创建文件夹
    {
        cout<<"创建文件夹";
        for(int i=0;i<despath.length();i++)//despath存储服务器端端的文件夹列表 在上方渲染文件结构时接收
        {
            //QString temppath=fpath.replace(0,abpd.length(),abps);这样会修改fpath的值 导致子文件比对失败 无法添加
            QString temppath=despath[i];
            temppath.replace(0,abpd.length(),abps);
            QDir ff(temppath);//创建不存在的文件夹
            cout<<"\n源文件路径"<<despath[i]<<"\n当前文件路径\n"<<temppath;//replace语句自带赋值属性 直接使用会导致下面的比对失败
            if(!ff.exists())
            {
                cout<<"addnew dir";
                cout<<ff.mkdir(temppath);
                extradir.append(temppath);
            }
        }
    }
}
void mythread2::checkedl()//移除之前创建的新文件夹
{
    cout<<"空文件夹路径为";
    cout<<extradir;
    if(extradir.length()!=0)
    {
        for(int i=extradir.length()-1;i>=0;i--)//文件夹存在层级关系 序号靠后的层级越低 应优先删除层级低的文件夹
        {
            QDir ff(extradir[i]);
            ff.rmdir(extradir[i]);
        }
        extradir.clear();
        //tsk->write("#destorydir#");//通知服务器删除
        //不通知服务器
    }
}
