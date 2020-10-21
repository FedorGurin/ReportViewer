#ifndef CREATEPDF_H
#define CREATEPDF_H

#include <QDialog>

namespace Ui {
class createpdf;
}

class createpdf : public QDialog
{
    Q_OBJECT

public:
    explicit createpdf(QWidget *parent = 0);
    ~createpdf();

private:
    Ui::createpdf *ui;
};

#endif // CREATEPDF_H
