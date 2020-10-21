#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QWidget>
#include <QComboBox>
#include <QColorDialog>
#include <QPen>
#include <QListWidgetItem>

namespace Ui {
class graphsettings;
}

typedef struct _TStateSettings
{
    QColor curr_col;
    QString paramname;
    int pointtype;
    //
    int isOptimize;//0=off. 1=on
    int isOptimize_prev;
    bool isDeltaActive;
    double deltaOptimize;//|x0-xi|<delta => xi aren't painted
    double deltaOptimize_prev;
}TStateSettings;

class graphsettings : public QWidget
{
    Q_OBJECT

public:
    explicit graphsettings(QWidget *parent = 0, QColor = Qt::gray);
    ~graphsettings();

    void initial(QColor, int, QString, QString);
    //! Для общих настроек
    void initialGeneralSettings();
    bool isGeneralSettings;
    static QList<graphsettings*> *listOfSettings;

    void setColor(QColor col);

    QList<QString> pointtypes;
    void setGraphPointTypes();

    QString path;
    TStateSettings workState;
    TStateSettings activeState;
    void setState(TStateSettings);

public slots:
    void slotColorDialog();
    void slotColorDialog(bool);
    void slotItemClicked(QListWidgetItem*);
    void slotComboBoxActivated(int);
    void slotParamNameChanged(QString);
    void slotRadioButton();
    void slotCheckBox(bool);
    void slotLineEditCbox();
    void slotOK();
    void slotApply();
    void slotClose();
signals:
    void signalChangeColorGraph(QColor);
    void signalChanePointTypeGraph(int);
    void signalParamNameChanged(QString);
    void signalOptimizeChanged(int, double);

private:
    Ui::graphsettings *ui;
};

#endif // GRAPHSETTINGS_H
