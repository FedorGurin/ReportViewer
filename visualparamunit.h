#ifndef VISUALPARAMUNIT_H
#define VISUALPARAMUNIT_H

#include "qcustomplotpro.h"
#include "graphsettings.h"

const QString aboveLayerName = "axes";//"aboveLayer";
const bool axesVisibility = false;

typedef struct _Tpoint
{
    double t;
    double val;
}Tpoint;

class QLabelPro : public QLabel
{
    Q_OBJECT
public:
    QLabelPro():QLabel(){}
    virtual ~QLabelPro(){}

    void mousePressEvent ( QMouseEvent * ev )
    {
        if(QApplication::mouseButtons() == Qt::LeftButton)
            emit signalPress(true);
        QLabel::mousePressEvent(ev);
    }

signals:
    void signalPress(bool);
};

//class MousePressEater : public QObject
//{
//    Q_OBJECT
//public:
//    MousePressEater(QObject *parent):QObject(parent){}
//    ~MousePressEater(){}
//protected:
//    bool eventFilter(QObject *obj, QEvent *event)
//    {
//        QCPGraph* gr = (QCPGraph*)obj;
//        if (event->type() == QEvent::MouseButtonPress)
//        {
//            if(QApplication::mouseButtons() == Qt::LeftButton)
//                emit gr->signalPress(true);
//            return true;
//        }
//        else // standard event processing
//            return QObject::eventFilter(obj, event);
//    }
//};

class visualParamUnit : public QObject
{
    Q_OBJECT
public:
    visualParamUnit(QCPAxisPro*, QCPGraph*, QCPItemTracer*, QCPItemText*);
    virtual ~visualParamUnit();

    int downloadPoints(QString filename, QString paramname, QString _measure, const Tpoint*, int size);

    QCPGraph* getGraph(){return graph;}
    QLineEdit* getlineEditCoordY();
    QCheckBox* getCheckBoxGraphs();
    QCPItemText* getItemTextCurrCoordY();
    QCPAxisPro* getAxis(){return yAxis;}
    //
    QLineEdit* edit_val;
    QCheckBox* cbox;
    QLabelPro* label_name;
    QLabel* label_color;
    QLabel* label_measure;
    //
//    graphstyle* graphStyle;
    graphsettings* graphSettings;

    QCPItemTracer* tracer;
    QCPItemText* currTracerValue;
    double curr_val;
    void changeGraphColor(QColor);
    void changeGraphColor(int r, int g, int b, int a);

    void changeScatterStyle();

private:
    QList<Tpoint> points_out;
    QList<Tpoint> points;
    QList<Tpoint> points_optimize;
    double dX_optimize;
    QList<Tpoint> optimizeDataMaker(double);
    int optimizePoints(QList<Tpoint>);

    QCPGraph* graph;
    QCPAxisPro* yAxis;
    //
    //QCPAxisPro* axis;

    void setSelectLabelName(bool);
public slots:
    void slotSelectGraphProgram(bool);//выделение графика программно, без пользовательского нажатия на график.
    void slotSelectionChangedByUser();
    void slotCbox(bool);    
    void slotChangeColorGraph(QColor);
    void slotChanePointTypeGraph(int);
    void slotParamNameChanged(QString);
    void slotOptimizeChanged(int,double);
private slots:
    void slotCboxMenu(QPoint);
    void slotVisibleTracer(bool b);
signals:
    void signalDeleteFromList(visualParamUnit* );
    void signalReplotCustomPlot(visualParamUnit*);
    void signalReplotCurrAxes();

    void signalCboxAllChange();
};

#endif // VISUALPARAMUNIT_H
