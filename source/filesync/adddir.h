#ifndef ADDDIR_H
#define ADDDIR_H

#include <QDialog>

namespace Ui {
class adddir;
}

class adddir : public QDialog
{
    Q_OBJECT

public:
    explicit adddir(QWidget *parent = nullptr);
    ~adddir();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::adddir *ui;
signals:
    void senddir(QString dir);
};

#endif // ADDDIR_H
