#ifndef FILETIME_H
#define FILETIME_H

#include <QObject>
#include <QFileInfo>
#include <QDateTime>//对应lastmodified为datatime对象
class filetime : public QObject
{
    Q_OBJECT
public:
    explicit filetime(QObject *parent = nullptr);
    QString getlastmodify(QString path);
    void setlastmodify(QString path,QString targettime);
signals:

public slots:
};

#endif // FILETIME_H
