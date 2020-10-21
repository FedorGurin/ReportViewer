#include "visualparamunit.h"

visualParamUnit::visualParamUnit(QCPAxisPro* _yAxis, QCPGraph* _graph,
                                 QCPItemTracer* _tracer, QCPItemText* _currTracerValue) : QObject()
{
    cbox = new QCheckBox();
    edit_val = new QLineEdit();
    label_name = new QLabelPro();
    label_color = new QLabelPro();
    label_measure = new QLabel();

    edit_val->setReadOnly(true);
    edit_val->setMaximumHeight(20);
    edit_val->setMaximumWidth(120/*60*/);
    edit_val->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum );


    yAxis = _yAxis;
    graph = _graph;

    points.clear();
    points_out.clear();

    //! Трассировка
    tracer = _tracer;
    tracer->setVisible(false);
    tracer->setLayer(aboveLayerName);
    tracer->setStyle(QCPItemTracer::tsSquare);//вид трассировки
    tracer->setInterpolating(true);
    currTracerValue = _currTracerValue;
    currTracerValue->setBrush(QBrush(Qt::white));
    currTracerValue->setVisible(false);
    currTracerValue->setLayer(aboveLayerName);

    label_name->setContextMenuPolicy(Qt::CustomContextMenu);
//    label_name->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
    connect(cbox, SIGNAL(clicked(bool)), this, SLOT(slotCbox(bool)));
    connect(label_name, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(slotCboxMenu(QPoint)));
    connect(label_name, SIGNAL(signalPress(bool)), this, SLOT(slotSelectGraphProgram(bool)));

    graphSettings = new graphsettings();
    connect(graphSettings, SIGNAL(signalChangeColorGraph(QColor)), this, SLOT(slotChangeColorGraph(QColor)));
    connect(graphSettings, SIGNAL(signalChanePointTypeGraph(int)), this, SLOT(slotChanePointTypeGraph(int)));
    connect(graphSettings, SIGNAL(signalParamNameChanged(QString)), this, SLOT(slotParamNameChanged(QString)));
    connect(graphSettings, SIGNAL(signalOptimizeChanged(int,double)), this, SLOT(slotOptimizeChanged(int,double)));
//    connect(graphSettings, SLOT(close())
//    label_name->set

    connect(label_color, SIGNAL(signalPress(bool)), graphSettings, SLOT(slotColorDialog(bool)));

    dX_optimize = 0.0;
    points_optimize.clear();
}

visualParamUnit::~visualParamUnit()
{
    graphSettings->close();
    cbox->deleteLater();
    edit_val->deleteLater();
    label_name->deleteLater();
    label_color->deleteLater();
    label_measure->deleteLater();
}

//default graph
int visualParamUnit::downloadPoints(QString filename, QString paramname, QString _measure, const Tpoint* poi, int size)
{
    for(int i = 0; i < size; i++)
    {
        Tpoint po = poi[i];//.t;
        points.append(po);
    }

    QVector<double> x(points.size()), y(points.size());
    for(double i = 0; i < points.size(); i++)
    {
        x[i] = points.at(i).t;
        y[i] = points.at(i).val;
    }

    graph->setData(x,y);
    //! произвольный выбор цвета графика
    int a = rand(), b = rand(), c = rand();
    changeGraphColor(a, b, c, 255);
    //! Стиль точек на графике
    QPen pen;
    pen.setColor(QColor(a%255, b%255, c%255));
    int dec = rand();
    graph->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(dec%14+1)));

    //! cbox
    //cbox->setChecked(true);
    slotCbox(false);

    edit_val->setText(QString(""));
    //static int num = 0;
    label_name->setText(paramname);//(QString("param %1").arg(num++));
    label_name->setToolTip(filename);
    label_measure->setText(_measure);
    tracer->setGraph(graph);
    graphSettings->initial(pen.color(), graph->scatterStyle().shape(), paramname, filename/*path*/);

    points_out = points;

    return 1;
}

int visualParamUnit::optimizePoints(QList<Tpoint> points_out)
{
    QVector<double> x(points_out.size()), y(points_out.size());
    for(double i = 0; i < points_out.size(); i++)
    {
        x[i] = points_out.at(i).t;
        y[i] = points_out.at(i).val;
    }

    graph->data().clear();
    graph->setData(x,y);

    return 1;
}

void visualParamUnit::slotCbox(bool b)
{    
    graph->setVisible(b);
    graph->valueAxis()->setVisible(b);
    if(b == false && graph->selected())//снимать выделение графика, когда он становится невидимым.
    {
        slotSelectGraphProgram(true);//после изменения графика - вызываются остальные слоты для других объектов QCP.
        tracer->setVisible(false);
    }

    emit signalReplotCurrAxes();
    //emit signalReplotCustomPlot(this);
}

void visualParamUnit::slotCboxMenu(QPoint pos)
{
    QPoint globalPos = label_name->mapToGlobal(pos);
    QMenu menu;
//    QAction* act1 = menu.addAction("Поменять цвет...");
//    QAction* act2 = menu.addAction("Поменять стиль точек...");
    QAction* act_tools = menu.addAction("Настройки графика...");
    QAction* act3 = menu.addAction("Удалить");
    QAction * selectedItem = menu.exec(globalPos);
    if(selectedItem)
    {
/*        if(selectedItem == act1)
        {
            changeColor();
        }
        else if(selectedItem == act2)
        {
            changeScatterStyle();
        }
        else */if(selectedItem == act_tools)
        {
            graphSettings->show();
        }
        else if(selectedItem == act3)
        {
            emit signalDeleteFromList(this);
        }
    }
}

void visualParamUnit::slotSelectGraphProgram(bool b)
{
    Q_UNUSED(b);

    if(cbox->isChecked())
    {
        if(!graph->selected())
        {
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            graph->valueAxis()->setSelectedParts(QCPAxisPro::spAxis|QCPAxisPro::spTickLabels);
            graph->valueAxis()->grid()->setVisible(true);
        }
        else
        {
            graph->setSelection(QCPDataSelection(QCPDataRange(0, 0)));
            graph->valueAxis()->setSelectedParts(QCPAxisPro::spNone);
            graph->valueAxis()->grid()->setVisible(false);
        }
        setSelectLabelName(graph->selected());
        slotVisibleTracer(graph->selected());
    }
    else
    {
        if(graph->selected())
        {
            graph->setSelection(QCPDataSelection(QCPDataRange(0, 0)));
            graph->valueAxis()->setSelectedParts(QCPAxisPro::spNone);
            graph->valueAxis()->grid()->setVisible(false);
            setSelectLabelName(false);
            slotVisibleTracer(false);
        }
    }
    emit signalReplotCustomPlot(this);
}

void visualParamUnit::setSelectLabelName(bool isSelected)
{
    QFont fnt;
    fnt.setBold(isSelected);
    label_name->setFont(fnt);
}

void visualParamUnit::slotSelectionChangedByUser()
{
    if(graph->selected())
    {
        graph->valueAxis()->setSelectedParts(QCPAxisPro::spAxis|QCPAxisPro::spTickLabels);
        graph->valueAxis()->grid()->setVisible(true);
        setSelectLabelName(true);
        slotVisibleTracer(true);
    }
    else if(graph->valueAxis()->selectedParts() == QCPAxisPro::spAxis
            || graph->valueAxis()->selectedParts() == QCPAxisPro::spTickLabels )
    {
        graph->valueAxis()->setSelectedParts(QCPAxisPro::spAxis|QCPAxisPro::spTickLabels);
        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        graph->valueAxis()->grid()->setVisible(true);
        setSelectLabelName(true);
        slotVisibleTracer(true);
    }
    else
    {
        graph->valueAxis()->grid()->setVisible(false);
        graph->valueAxis()->setSelectedParts(QCPAxisPro::spNone);
        setSelectLabelName(false);
        slotVisibleTracer(false);
    }
}

void visualParamUnit::slotVisibleTracer(bool b)
{
    tracer->setVisible(b);
    currTracerValue->setVisible(b);
}

void visualParamUnit::slotChangeColorGraph(QColor new_col)
{
//    QColorDialog dialog;
//    QColor new_col = dialog.getColor(graph->pen().color());
    if(new_col.isValid())
    {
        changeGraphColor(new_col);
        emit signalReplotCustomPlot(this);
    }
}

void visualParamUnit::slotChanePointTypeGraph(int pointtype)
{
    if(pointtype > -1 && pointtype < 16)
    {
        graph->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(pointtype)));
        emit signalReplotCustomPlot(this);
    }
}

void visualParamUnit::changeGraphColor(QColor col)
{
    QPen pen;
    pen.setColor(col);
    graph->setPen(pen);
    graph->valueAxis()->setBasePen(graph->pen());

    int col_r,col_g,col_b; pen.color().getRgb(&col_r,&col_g,&col_b);
    QFont fnt1;
    fnt1.setBold(true);
    label_color->setStyleSheet(QString("background: rgb(%1, %2, %3)").arg(col_r).arg(col_g).arg(col_b));
    label_color->setFont(fnt1);

    pen.setWidth(3);
    QCPSelectionDecorator* decor = new QCPSelectionDecorator();
    decor->setPen(pen);
//    graph->setSelectionDecorator(decor);
    graph->valueAxis()->setSelectedBasePen(pen);
    graph->valueAxis()->setSelectedSubTickPen(pen);
    graph->valueAxis()->setSelectedTickPen(pen);
    graph->valueAxis()->setSelectedTickLabelColor(Qt::black);

    QCPScatterStyle myScatter;
    myScatter.setShape(graph->scatterStyle().shape());
    myScatter.setPen(pen);
    decor->setScatterStyle(myScatter);
    graph->setSelectionDecorator(decor);
}

void visualParamUnit::changeGraphColor(int r, int g, int b, int a = 255)
{
    QPen pen;
    pen.setColor(QColor(r%a, g%a, b%a));
    graph->setPen(pen);
    graph->valueAxis()->setBasePen(graph->pen());

    QFont fnt1;
    fnt1.setBold(true);
    label_color->setStyleSheet(QString("background: rgb(%1, %2, %3)").arg(r%a).arg(g%a).arg(b%a));
    label_color->setFont(fnt1);

    pen.setWidth(3);
    QCPSelectionDecorator* decor = new QCPSelectionDecorator();
    decor->setPen(pen);
//    graph->setSelectionDecorator(decor);
    graph->valueAxis()->setSelectedBasePen(pen);
    graph->valueAxis()->setSelectedSubTickPen(pen);
    graph->valueAxis()->setSelectedTickPen(pen);
    graph->valueAxis()->setSelectedTickLabelColor(Qt::black);

    QCPScatterStyle myScatter;
    myScatter.setShape(graph->scatterStyle().shape());
    myScatter.setPen(pen);
    decor->setScatterStyle(myScatter);
    graph->setSelectionDecorator(decor);
}

void visualParamUnit::slotParamNameChanged(QString new_name)
{
    label_name->setText(new_name);
}

void visualParamUnit::changeScatterStyle()
{
    graphSettings->show();
}

QList<Tpoint> visualParamUnit::optimizeDataMaker(double dX)
{
    points_optimize.clear();
    points_optimize.append(points.at(0));
    for(double i = 0; i < points.size()-1; i++)
    {
        if(fabs(points.at(i).val - points_optimize.at(points_optimize.size()-1).val) > dX)
        {
            if(points.at(i).t - points_optimize.at(points_optimize.size()-1).t > 10e-5)
                points_optimize.append(points.at(i-1));
            points_optimize.append(points.at(i));
        }
    }
    points_optimize.append(points.at(points.size()-1));
    return points_optimize;
}

void visualParamUnit::slotOptimizeChanged(int isOpt, double dX)
{
    if(isOpt == 0)
    {
        optimizePoints(points);
    }
    else
    {
        if(dX != dX_optimize)
        {
            optimizeDataMaker(dX);
            optimizePoints(points_optimize);
        }
    }
    emit signalReplotCustomPlot(this);
}

