#ifndef CHOOSEPARAMSFROMFILE_H
#define CHOOSEPARAMSFROMFILE_H

#include <QDialog>
#include <QAbstractButton>
#include <QMessageBox>

namespace Ui {
class chooseParamsFromFile;
}

class chooseParamsFromFile : public QDialog
{
    Q_OBJECT

public:
    explicit chooseParamsFromFile(QWidget *parent = 0);
    ~chooseParamsFromFile();

    void setPath(QString);
    void setParamNames(QList<QString>);

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::chooseParamsFromFile *ui;
    QString filename;
signals:
    void signalAddGraphsFromFile(QString,QList<int>);
};

#endif // CHOOSEPARAMSFROMFILE_H
