#include "createpdf.h"
#include "ui_createpdf.h"

createpdf::createpdf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createpdf)
{
    ui->setupUi(this);
}

createpdf::~createpdf()
{
    delete ui;
}
