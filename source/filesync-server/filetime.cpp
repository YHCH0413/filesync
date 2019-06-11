#include "filetime.h"
#include <QDebug>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"

#include <stdlib.h>
#include <sys/utime.h>

filetime::filetime(QObject *parent) : QObject(parent)
{

}
QString filetime::getlastmodify(QString path)
{
    QFileInfo file(path);
    QDateTime dtime=file.lastModified();

    int year=dtime.date().year();
    int month=dtime.date().month();
    int day=dtime.date().day();
    int hour=dtime.time().hour();
    int minute=dtime.time().minute();
    int second=dtime.time().second();

    return QString::number(year)+"#"+QString::number(month)+"#"+QString::number(day)+"#"+QString::number(hour)+"#"+QString::number(minute)+"#"+QString::number(second);
}
void filetime::setlastmodify(QString path, QString targettime)
{

//    struct tm tma = {0};
    // Fill out the accessed time structure
//    tma.tm_hour = 12;
//    tma.tm_isdst = 0;
//    tma.tm_mday = 15;
//    tma.tm_min = 0;
//    tma.tm_mon = 0;
//    tma.tm_sec = 0;
//    tma.tm_year = 103;
//ut.actime = mktime(&tma);
    //抛弃访问时间 这里只使用最后修改时间
    cout<<targettime.section("#",0,0).toInt()<<targettime.section("#",1,1).toInt()<<targettime.section("#",2,2).toInt()<<targettime.section("#",3,3).toInt()<<targettime.section("#",4,4).toInt()<<targettime.section("#",5,5).toInt();
    struct tm tmm = {0};
    struct _utimbuf ut;
    // Fill out the modified time structure
    tmm.tm_year = targettime.section("#",0,0).toInt()-1900;
    tmm.tm_mon = targettime.section("#",1,1).toInt()-1;
    tmm.tm_mday = targettime.section("#",2,2).toInt();
    tmm.tm_hour = targettime.section("#",3,3).toInt();
    tmm.tm_min = targettime.section("#",4,4).toInt();
    tmm.tm_sec = targettime.section("#",5,5).toInt();
    tmm.tm_isdst = 0;//非夏令时

    // Convert tm to time_t

    ut.modtime = mktime(&tmm);

    // Show file time before and after
    if(_utime(path.toLocal8Bit(),&ut )>-1)
    {
        cout<<"设定成功";
    }
    else {
        cout<<"设定失败";
        system("PAUSE");
    }
}
