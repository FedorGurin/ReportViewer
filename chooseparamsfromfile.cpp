#include "chooseparamsfromfile.h"
#include "ui_chooseparamsfromfile.h"

chooseParamsFromFile::chooseParamsFromFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseParamsFromFile)
{
    ui->setupUi(this);
    setWindowTitle(QString("Загрузка из файла..."));
//    setWindowIcon(parent->windowIcon());
    setWindowIcon(QIcon(":/open_folder"));
    ui->lineEdit->setReadOnly(true);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
}

chooseParamsFromFile::~chooseParamsFromFile()
{
    delete ui;
}

void chooseParamsFromFile::setPath(QString text)
{
    filename = text;
    ui->lineEdit->setText(filename);
}

void chooseParamsFromFile::setParamNames(QList<QString> list)
{
    ui->listWidget->clear();
    for(int i = 0; i < list.size(); i++)
    {
        ui->listWidget->addItem(list.at(i));
    }
}

void chooseParamsFromFile::on_pushButton_ok_clicked()
{
    QList<int> mas;
    mas.clear();
    for(int i = 0; i < ui->listWidget->count(); i++)
        if(ui->listWidget->item(i)->isSelected())
            mas.append(i+1);
    if(mas.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Не выбран ни один параметр.");
        msgBox.exec();
    }
    else
    {
        emit signalAddGraphsFromFile(filename, mas);
        close();
    }

}

void chooseParamsFromFile::on_pushButton_cancel_clicked()
{
    close();
}
