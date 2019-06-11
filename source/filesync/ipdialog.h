#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>

namespace Ui {
class ipdialog;
}

class ipdialog : public QDialog
{
    Q_OBJECT

public:
    explicit ipdialog(QWidget *parent = nullptr);
    ~ipdialog();

private slots:
    void on_pushButton_clicked();
signals:
    void reconnect();
private:
    Ui::ipdialog *ui;
};

#endif // IPDIALOG_H
