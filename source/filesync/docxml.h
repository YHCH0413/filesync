#ifndef DOCXML_H
#define DOCXML_H
#include <QString>
#include <QDomDocument>
class docxml
{
public:
    docxml();
    static void createxml(QString path);
    static void addelement(QString path,QString cont);
    static QStringList readelement(QDomDocument doc);
};

#endif // DOCXML_H
