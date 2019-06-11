#include "mywidget.h"
#include <QApplication>
#include <QFile>
#include "dialogwd.h"
#include "filetime.h"
#include "ipdialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QStringList as;
//    as<<"a"<<"b"<<"c";
//    Dialog dia(as,1);
//    dia.show();
//    return 3;
//    filetime ft;
//    ft.setlastmodify("2.txt","2018#5#30#9#12#0");
    ipdialog id;
    mywidget w(&id);
    w.show();
    QFile file("ip.txt");
    if(!file.exists())
    {
        id.show();

    }
    else
    {
        if(file.open(QIODevice::ReadOnly))
        {
            if(file.readLine()=="")
            {
                file.close();
                id.show();

            }
        }
    }
    return a.exec();
}
