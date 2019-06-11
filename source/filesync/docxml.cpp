#include "docxml.h"
#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QDomProcessingInstruction>
#include <QStringList>
#define cout qDebug()<<"["<<__FILE__<<__LINE__<<"]"
docxml::docxml()
{

}
void docxml::createxml(QString path)
{
    QFile file(path);
    if(file.exists())
    {
        cout<<"already exits";
        return;
    }
    else
    {
       bool succ=file.open(QIODevice::WriteOnly);
       if(succ)
       {
          QDomDocument doc;
          QDomProcessingInstruction ins;
          ins=doc.createProcessingInstruction("xml","version=\'1.0\' encoding=\'utf-8\'");
          doc.appendChild(ins);

          QDomElement root=doc.createElement("path");
          doc.appendChild(root);

          QTextStream stream(&file);
          doc.save(stream,4);
          file.close();
       }
       else
       {
           cout<<"open failed";
       }
    }
}
void docxml::addelement(QString path,QString cont)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        bool succ=doc.setContent(&file);
        file.close();
        if(succ)
        {
            bool flag=true;
            QDomElement root=doc.documentElement();//获取根节点

            QDomElement content=doc.createElement("short-cut");
            QDomText text=doc.createTextNode(cont);

            QStringList slist=readelement(doc);
            for (int i=0;i<slist.length();i++)
            {
                if(slist[i]==cont)
                {
                    flag=false;
                    break;//不需要像词典程序一样将其放在最前面
                }
            }
            if(flag==false)//有重复
            {
                cout<<"docxml append already have this one";
            }
            else //没有重复
            {
                QDomNodeList nlist=root.childNodes();
                if(nlist.length()>=5)
                {
                    cout<<"test1";
                    root.removeChild(root.childNodes().at(0));//删除首个节点
                    content.appendChild(text);
                    root.appendChild(content);
                    //save
                    file.open(QIODevice::WriteOnly);
                    QTextStream stream(&file);
                    doc.save(stream,4);
                    file.close();
                }
                else
                {
                    cout<<"test2";
                    content.appendChild(text);
                    root.appendChild(content);
                    //save
                    file.open(QIODevice::WriteOnly);
                    QTextStream stream(&file);
                    doc.save(stream,4);
                    file.close();
                }
            }
        }
        else {
            cout<<"set content failed in add";
        }
    }
    else {
        cout<<"open failed in add";
    }
}
QStringList docxml::readelement(QDomDocument doc)
{
    QStringList list;
    QDomElement root=doc.documentElement();
    QDomNodeList doclist=root.childNodes();
    //获取根节点下的所有子节点 放入list中
    QString text;
    for (int i=0;i<doclist.length();i++)
    {
        text=doclist.at(i).toElement().text();
        list.append(text);
    }
    return  list;

}
