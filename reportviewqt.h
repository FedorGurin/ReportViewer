#ifndef REPORTVIEWQT_H
#define REPORTVIEWQT_H

#include <QMainWindow>

#include "visualparamunit.h"
#include "chooseparamsfromfile.h"
#include "createpdf.h"

#include <QTableWidgetItem>

namespace Ui {
class reportviewQt;
}

#define MAX_NUMBER_OF_AXES 10

class reportviewQt : public QMainWindow
{
    Q_OBJECT

public:
    explicit reportviewQt(QWidget *parent = 0);
    virtual ~reportviewQt();

    QIcon windowIcon;

    bool event(QEvent *e);

    void readParamNamesFromFile(QString);
    QList<QString> listParamNamesFromFile;
    QList<QString> listMeasuresFromFile;
    QTextCodec *textCodec;
    QString previousFilePath;

    createpdf* createPdf;

//    void createDockWindows();
    QDockWidget *dock_Params;
    QScrollArea *scrollArea;
    QWidget* widgParams;
    QGridLayout* gridLayout_params;
    QTableWidget* tabWidget;

private:
    Ui::reportviewQt *ui;

    chooseParamsFromFile* formSelectParamsFromFile;
    //! Вертикальная линия для отображения значений разных переменных в один и тот же момент времени.
    QCPCurve* verticalLine;
    QCPItemText* currTime;//текущее время на груфике
    // для отображения приращения по времени
    QCPCurve* verticalLine_2;
    QCPItemText* currTime_2;
    QCPItemBracket* bracketTime;
    QCPItemText* deltaT;

    //! Текущая координата по оси х для отрисовки вертикальной линии и трассировки.
    double coordX;

    // Добавление графика.
    void addGraph();
    void addGraph(QString filename, int column);

    /////////////////
    QList<visualParamUnit*> listParams;
    //
    QList< QCPAxis *> getAllAxis();
    QList< QCPGraph *> getSelectedGraphs();

    //
    graphsettings* generalSettings;

signals:
    void signalReplotListParams();
private slots:
    void slotMousePress(QMouseEvent* event);
    void slotMouseMove(QMouseEvent* event);
    void slotMouseWheel(QWheelEvent *event);
//    void slotSelectionChanged();
    // Отрисовка вертикальной линии в соответствии с размерами окна и действиями пользователя.
    void slotReplotVertLine(){}
    void slotReplotVertLine(QCPCurve* vLine, QCPItemText* itemText);
    void slotReplotListParams();
    void slotDeleteFromList(visualParamUnit*);
    void slotDeleteFromList(QList<visualParamUnit*> list_vp);
    void slotReplotCurrAxes();
    void slotAllGraphsVisible();
    void slotAllGraphsInvisible();
    void slotBeforeReplot();
    void slotClearWidget();
    void slotDownloadFile();
    void slotDataParser();
    void slotTools();
    void slotAddGraphFromFile(QString, QList<int>);
//    void on_action_write_to_pdf_clicked();
    void slotScrollAreaMenu(QPoint);
    void slotAboutThis();
};


#endif // REPORTVIEWQT_H
