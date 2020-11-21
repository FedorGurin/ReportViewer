#include "reportviewqt.h"
#include "ui_reportviewqt.h"

#include <QTextCodec>

#define DEBUG_FLAG 0

//Q_INIT_RESOURCE(icons)

reportviewQt::reportviewQt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::reportviewQt)
{
    /*QTextCodec *utfcodec =*/ QTextCodec::codecForName("UTF-8");
    textCodec = QTextCodec::codecForName("UTF-8");//QTextCodec::codecForName("KOI8-R");//QTextCodec::codecForName("windows-1251");

    ui->setupUi(this);

    setWindowTitle(QString("Просмотр графиков"));
    setWindowState(Qt::WindowMaximized);

    //    setWindowIcon(QIcon(QString(":/face-smile")));
    //    setWindowIcon(QIcon(QString(":/bomb")));
    windowIcon = QIcon(QString(":/bomb"));
    setWindowIcon(windowIcon);

    listParams.clear();
    srand(QDateTime::currentDateTime().toTime_t());
    coordX = 0.;
    //! Инициализация осей
    ui->customplot->xAxis->setLabel("Время, с");
//    ui->customplot->yAxis->setSelectableParts(QCPAxis::spNone);
    ui->customplot->yAxis2->setSelectableParts(QCPAxis::spNone);
    ui->customplot->xAxis2->setSelectableParts(QCPAxis::spNone);
    ui->customplot->yAxis->grid()->setVisible(false);
    ui->customplot->yAxis->setVisible(false);
    ui->customplot->yAxis->setTickLength(0,0);
    ui->customplot->yAxis->setSubTickLength(0,0);
    ui->customplot->yAxis2->setTickLength(0,0);
    ui->customplot->yAxis2->setSubTickLength(0,0);
    ui->customplot->yAxis2->grid()->setVisible(false);
    ui->customplot->yAxis2->setVisible(false);

    ui->customplot->xAxis->setRangeLower(0);
    ui->customplot->yAxis->setSelectedTickLabelColor(Qt::white);

    //! Отрисовка всех четырех осей по периметру.
    ui->customplot->axisRect()->setupFullAxesBox();
    ui->customplot->axisRect()->axis(QCPAxis::atLeft)->setTickLabelColor(Qt::white);
    //! Свойства QCustemPlot, необходимые для работы с графиками с интерактивном режиме.
    ui->customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->customplot->setInteraction(QCP::iMultiSelect);
    //! Для работы с легендой при создании отчета
    ui->customplot->legend->setVisible(false);
//    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
//    ui->customplot->plotLayout()->addElement(0,0,subLayout);
//    subLayout->setMargins(QMargins(ui->customplot->rect().left(),ui->customplot->rect().top(),
//                                   ui->customplot->rect().right(),ui->customplot->rect().bottom()));

    //! Вертикальная линия для отображения текущих значений графиков
    verticalLine = new QCPCurve(ui->customplot->xAxis, ui->customplot->yAxis);
    verticalLine->setName("verticalLine");
    verticalLine->removeFromLegend();
    QPen pen(Qt::black);
    pen.setWidth(1);
    verticalLine->setPen(pen);
    QVector<double> x(2), y(2);
        x[0] = x[1] = 0.;
        y[0] = ui->customplot->yAxis->range().lower;
        y[1] = ui->customplot->yAxis->range().upper;
    verticalLine->setData(x,y);
    verticalLine->setSelectable(QCP::stNone);
    verticalLine->setLayer(aboveLayerName);
    //! Текущее время (отображение вертикальной линии)
    currTime = new QCPItemText(ui->customplot);
    currTime->setVisible(true);
    currTime->setLayer(aboveLayerName);
    currTime->setBrush(QBrush(Qt::white));

    //! Вертикальная линия для отображения интервала времени!
    verticalLine_2 = new QCPCurve(ui->customplot->xAxis, ui->customplot->yAxis);
    verticalLine_2->setName("verticalLine_2");
    verticalLine_2->removeFromLegend();
    verticalLine_2->setPen(pen);
    verticalLine_2->setData(x,y);
    verticalLine_2->setSelectable(QCP::stNone);
    verticalLine_2->setLayer(aboveLayerName);
    //! Текущее время (отображение вертикальной линии)
    currTime_2 = new QCPItemText(ui->customplot);
    currTime_2->setVisible(true);
    currTime_2->setLayer(aboveLayerName);
    currTime_2->setBrush(QBrush(Qt::white));
    verticalLine_2->setVisible(false);
    currTime_2->setVisible(false);
    bracketTime = new QCPItemBracket(ui->customplot);
    bracketTime->setVisible(false);
    deltaT = new QCPItemText(ui->customplot);
    deltaT->setVisible(false);
    deltaT->setLayer(aboveLayerName);
    deltaT->setBrush(QBrush(Qt::white));

//    ui->customplot->rescaleAxes();
    ui->customplot->replot();

    createPdf = new createpdf();
//    connect(createPdf, SIGNAL())

    ui->action_write_to_pdf->setEnabled(false);

    ////
//    ui->statusBar

    generalSettings = new graphsettings();
    generalSettings->initialGeneralSettings();
//    generalSettings->set

    //! MainMenu
    ui->action_general_settings->setEnabled(true);
    ui->menu_window->setEnabled(false);
    ui->action_help->setEnabled(false);

    //! mainToolBar
    ui->mainToolBar->setWindowTitle(QString("Панель быстрого доступа"));
    //! 1)
    ui->mainToolBar->addSeparator();
    QIcon icon_open_folder = QIcon(QString(":/open_folder"));
    ui->action_downloadFile->setIcon(icon_open_folder);
    QAction* act_readFromFile = new QAction(icon_open_folder,QString("Загрузить данные из файла"), this);
    connect(act_readFromFile, SIGNAL(triggered()), this, SLOT(slotDownloadFile()));
    ui->mainToolBar->addAction(act_readFromFile);

    QIcon icon_dataparser = QIcon(":/paste");
    ui->action_dataParser->setIcon(icon_dataparser);
    QAction* act_dataParser = new QAction(icon_dataparser,QString("Конвертер данных"), this);
    connect(act_dataParser, SIGNAL(triggered()), this, SLOT(slotDataParser()));
    ui->mainToolBar->addAction(act_dataParser);

    QIcon icon_tools = QIcon(":/icon_Tools");
    ui->action_general_settings->setIcon(icon_tools);
    QAction* act_Tools = new QAction(icon_tools,QString("Общие настройки"), this);
    act_Tools->setEnabled(true);
    connect(act_Tools, SIGNAL(triggered()), this, SLOT(slotTools()));
    ui->mainToolBar->addAction(act_Tools);
    ui->mainToolBar->addSeparator();
    //! 2)
    QAction* act_check_all = new QAction(QIcon(":/box_checked_enabled"),QString("Показать все графики"), this);
    connect(act_check_all, SIGNAL(triggered()), this, SLOT(slotAllGraphsVisible()));
    ui->mainToolBar->addAction(act_check_all);
    QAction* act_uncheck_all = new QAction(QIcon(":/box_unchecked_enabled"),QString("Скрыть все графики"), this);
    connect(act_uncheck_all, SIGNAL(triggered()), this, SLOT(slotAllGraphsInvisible()));
    ui->mainToolBar->addAction(act_uncheck_all);
    ui->mainToolBar->addSeparator();
    //! 3)
    QIcon icon_delete = QIcon(":/standardbutton-delete-128");
    ui->action__clear_qcp->setIcon(icon_delete);
    QAction* act_remove_all = new QAction(icon_delete,QString("Удалить все графики"), this);
    connect(act_remove_all, SIGNAL(triggered()), this, SLOT(slotClearWidget()));
    ui->mainToolBar->addAction(act_remove_all);

    //! Окно "Параметры"
    //1) 1st variant
//    QDockWidget *dock_Params = new QDockWidget(tr("Параметры"), this);
//    QScrollArea* scrollArea = new QScrollArea();
//    scrollArea->setLayout();
//    dock_Params->setWidget(ui->scrollArea);
//    addDockWidget(Qt::LeftDockWidgetArea, dock_Params);
    //2) 2nd variant
    dock_Params = new QDockWidget(tr("Параметры"), this);
    dock_Params->setVisible(false);    
    scrollArea = new QScrollArea(dock_Params);
    scrollArea->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(scrollArea, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(slotScrollAreaMenu(QPoint)));
    scrollArea->setObjectName(QStringLiteral("scrollArea"));
    scrollArea->setGeometry(QRect(9, 9, 51, 321));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
    scrollArea->setSizePolicy(sizePolicy1);
    scrollArea->setMinimumSize(QSize(50, 0));
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    tabWidget = new QTableWidget(dock_Params);
    tabWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tabWidget->setGridStyle(Qt::NoPen);
    tabWidget->horizontalHeader()->setVisible(false);
    tabWidget->setRowCount(0);
    tabWidget->setColumnCount(5);
    tabWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tabWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);//ResizeToContents);
    tabWidget->setColumnWidth(0,20);//!
//    tabWidget->setColumnWidth(1,50);
    tabWidget->setColumnWidth(2,15);//!
//    tabWidget->setColumnWidth(4,30);
    scrollArea->setWidget(tabWidget);
    dock_Params->setWidget(scrollArea);

    addDockWidget(Qt::LeftDockWidgetArea, dock_Params);

//    dock_Params = new QDockWidget(tr("Параметры"), this);
//    widgParams = new QWidget;
//    gridLayout_params = new QGridLayout(widgParams);
//    dock_Params->setWidget(widgParams);
//    addDockWidget(Qt::LeftDockWidgetArea, dock_Params);

    formSelectParamsFromFile = new chooseParamsFromFile(this);
    listParamNamesFromFile.clear();
    listMeasuresFromFile.clear();
    connect(formSelectParamsFromFile, SIGNAL(signalAddGraphsFromFile(QString, QList<int>)), this, SLOT(slotAddGraphFromFile(QString, QList<int>)));
    previousFilePath.clear();

    connect(ui->customplot, SIGNAL(beforeReplot()), this, SLOT(slotBeforeReplot()));
    connect(ui->customplot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(slotMousePress(QMouseEvent*)));
    connect(ui->customplot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(slotMouseMove(QMouseEvent*)));
    connect(this, SIGNAL(signalReplotListParams()), this, SLOT(slotReplotListParams()));
    connect(ui->customplot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(slotMouseWheel(QWheelEvent*)));
//    connect(ui->customplot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
//    connect(ui->customplot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(slotMouseDoubleClick(QMouseEvent*)));
    connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_downloadFile, SIGNAL(triggered()), this, SLOT(slotDownloadFile()));
    connect(ui->action__clear_qcp, SIGNAL(triggered()), this, SLOT(slotClearWidget()));
    connect(ui->action_dataParser, SIGNAL(triggered()), this, SLOT(slotDataParser()));
    connect(ui->action_about_this, SIGNAL(triggered()), this, SLOT(slotAboutThis()));

#if(DEBUG_FLAG)
    addGraph();
    addGraph();
    dock_Params->setVisible(true);
#endif
}

reportviewQt::~reportviewQt()
{
    delete ui;
}

//Удаление всех окон при закрытии главного окна
bool reportviewQt::event(QEvent *e)
{
    if(e->type()==QEvent::Close)// закрыли это окно
    {
        //закрыть все производные окна
        for(int i = 0; i < listParams.size(); i++)
        {
            listParams.at(i)->graphSettings->close();
        }
        close();
    }
    return QMainWindow::event(e);
}

void reportviewQt::slotClearWidget()
{
    if(listParams.isEmpty())
        return;
    QMessageBox msgBox;
    msgBox.setWindowIcon(windowIcon);
    msgBox.setText("Вы, действительно, хотите стереть все графики? Параметры в списке тоже будут удалены. ");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Ok:
          // Save was clicked
           slotDeleteFromList(listParams);
          break;
      case QMessageBox::Cancel:
          // Cancel was clicked
          break;
      default:
          // should never be reached
          break;
    }
}

void reportviewQt::slotDownloadFile()
{
//    QStringList files = QFileDialog::getOpenFileNames(
//                            this,
//                            "Выберите файл...",
//                            "C:/MyProg/build-reportViewQt-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Debug/значения",//"/home",
//                            "Text files (*.txt, *.xlsx)");
//    if(!files.isEmpty())
//    {

//    }
    QString path = QApplication::applicationDirPath()+"/значения";
    QFileDialog dialog;
    QString file = dialog.getOpenFileName(
                            this,
                            "Выберите файл...",
                            path,
                            "Text files (*.txt)");
    if(!file.isEmpty())
    {
        //addGraph(file);
        readParamNamesFromFile(file);
        formSelectParamsFromFile->setPath(file);
        formSelectParamsFromFile->setParamNames(listParamNamesFromFile);
        formSelectParamsFromFile->show();
        previousFilePath = file;
    }
}

void reportviewQt::slotDataParser()
{
    QString program = QApplication::applicationDirPath()+"/dataparsertotxt";
    QFile file(QString(program+".exe"));
    if(file.exists())
    {
        QProcess *myProcess = new QProcess(this);
        myProcess->start(program);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowIcon(windowIcon);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(QString("Не удается найти "+file.fileName()));
        msgBox.exec();
    }
}

void reportviewQt::slotTools()
{
    generalSettings->show();
}

void reportviewQt::readParamNamesFromFile(QString filename)
{
    listParamNamesFromFile.clear();
    listMeasuresFromFile.clear();
    QFile* file = new QFile(filename);
    if(file->open(QIODevice::ReadOnly))
    {
//        file->readLine();
//        while(!file->atEnd())
//        {
            QByteArray line = file->readLine();
            QList<QByteArray> list = line.split(';');
//            map.insert(list.at(1).toDouble(),list.at(num_column_param_BINS).toDouble());
            //i = 0 - time
            for(int i = 1; i < list.size()-1; i++)
            {
                //if(list.at(i).contains())//if(!list.at(i).isEmpty())
                if(list.at(i).contains("[") && list.at(i).contains("]"))
                {
                    int a0 = list.at(i).indexOf("[");
                    int a1 = list.at(i).indexOf("]");
                    listMeasuresFromFile.append(list.at(i).mid(a0+1, a1-a0-1));
                    listParamNamesFromFile.append(textCodec->toUnicode(list.at(i).mid(0,a0)));
                }
                else
                {
                    listParamNamesFromFile.append(textCodec->toUnicode(list.at(i)));
                    listMeasuresFromFile.append(QString("ед."));
                }

            }
//        }
    }
    file->close();
}

void reportviewQt::slotAddGraphFromFile(QString filename, QList<int> list)
{
    for(int i = 0; i < list.size(); i++)
    {
        addGraph(filename, list.at(i));
    }
    dock_Params->setVisible(true);
}

void reportviewQt::addGraph(QString filename, int column)
{
    QVector<Tpoint> d;
    d.clear();
    QString paramName = listParamNamesFromFile.at(column-1);
//    QString measure("ед.");
    QFile* file = new QFile(filename);
    int N = 0;
    if(file->open(QIODevice::ReadOnly))
    {
        file->readLine();
        while(!file->atEnd())
        {
            N++;
            QByteArray line = file->readLine();
            QList<QByteArray> list = line.split(';');
            Tpoint temp_p;
            temp_p.t = list.at(0).toDouble();//i = 0 - time
            temp_p.val = list.at(column).toDouble();
            d.append(temp_p);
        }
    }
    file->close();

    //! Добавление графика; для каждого нового графика создается своя ось ординат, а ось абсцисс - общая для всех.
    QCPAxis* yAxis = (QCPAxis*)ui->customplot->axisRect()->addAxis(QCPAxis::atRight);//(QCPAxis::atLeft);
    visualParamUnit* newParam = new visualParamUnit((QCPAxisPro*)yAxis,
                                                    new QCPGraph(ui->customplot->xAxis, (QCPAxisPro*)yAxis),
                                                    new QCPItemTracer(ui->customplot), new QCPItemText(ui->customplot));
    newParam->downloadPoints(filename, paramName, listMeasuresFromFile.at(column-1)/*measure*/, &d.at(0), N);
    connect(newParam, SIGNAL(signalDeleteFromList(visualParamUnit*)), this, SLOT(slotDeleteFromList(visualParamUnit*)));
    connect(newParam, SIGNAL(signalReplotCustomPlot(visualParamUnit*)), ui->customplot, SLOT(replot()));
    connect(ui->customplot, SIGNAL(selectionChangedByUser()), newParam, SLOT(slotSelectionChangedByUser()));
    connect(newParam, SIGNAL(signalReplotCurrAxes()), this, SLOT(slotReplotCurrAxes()));

    listParams.append(newParam);
    emit signalReplotListParams();

//    yAxis->setRange(a,b);
    newParam->getGraph()->rescaleValueAxis();
    ui->customplot->replot();
}

void reportviewQt::addGraph()
{
    //! Добавление графика; для каждого нового графика создается своя ось ординат, а ось абсцисс - общая для всех.
    const int N = 10;
    Tpoint d[N];
    for(int i = 0; i < N; i++)
    {
        d[i].t = i*(listParams.size()+1);
        d[i].val = (listParams.size()+1) > i*0.3 ? 0 : listParams.size();
    }

    QCPAxis* yAxis = (QCPAxis*)ui->customplot->axisRect()->addAxis(QCPAxis::atRight);//(QCPAxis::atLeft);
    visualParamUnit* newParam = new visualParamUnit((QCPAxisPro*)yAxis,
                                                    new QCPGraph(ui->customplot->xAxis, (QCPAxisPro*)yAxis),
                                                    new QCPItemTracer(ui->customplot), new QCPItemText(ui->customplot));

    static int i = 0; i++; QString name = QString("default param %1").arg(i);
    QString measure("ед.");
    newParam->downloadPoints(QString("example"), name, measure, &d[0], N);
    connect(newParam, SIGNAL(signalDeleteFromList(visualParamUnit*)), this, SLOT(slotDeleteFromList(visualParamUnit*)));
    connect(newParam, SIGNAL(signalReplotCustomPlot(visualParamUnit*)), ui->customplot, SLOT(replot()));
    connect(ui->customplot, SIGNAL(selectionChangedByUser()), newParam, SLOT(slotSelectionChangedByUser()));
    connect(newParam, SIGNAL(signalReplotCurrAxes()), this, SLOT(slotReplotCurrAxes()));

    listParams.append(newParam);
    emit signalReplotListParams();

    //ui->customplot->rescaleAxes();
    newParam->getGraph()->rescaleValueAxis();
    ui->customplot->replot();
}
//СЛОТЫ
void reportviewQt::slotMouseMove(QMouseEvent* event)
{
    //! Обработка события, когда нажата правая кнопка мыши
    if(QApplication::mouseButtons() == Qt::RightButton)
    {
        coordX = ui->customplot->xAxis->pixelToCoord(event->pos().x());

        //
        for(int i = 0; i < listParams.size(); i++)
        {
            if(listParams.at(i)->getGraph()->selected())
            {
                QCPItemTracer* tracer = listParams.at(i)->tracer;
                //нахождение ближайшей точки, меньше данной
                tracer->setGraphKey(coordX);
                int indx = listParams.at(i)->getGraph()->findBegin(coordX,true);
                double y_graph = listParams.at(i)->getGraph()->data()->at(indx)->value;

//                QSharedPointer<QCPGraphDataContainer> pointer = listParams.at(i)->getGraph()->data();//.value->at(indx)->value;
//                //QCPGraphDataContainer *container =
//               // double y_graph =  pointer->at(indx)->value;
//                //double y_graph = container->value;
                listParams.at(i)->currTracerValue->setText(QString("%1").arg(y_graph/*tracer->position->value()*/));
                QPointF point = tracer->position->pixelPosition();
                point.setY(point.y()-10);
                point.setX(point.x()+15);
                listParams.at(i)->currTracerValue->position->setPixelPosition(point);
            }
        }
    }
    if(QApplication::mouseButtons() == Qt::RightButton
            && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        verticalLine_2->setVisible(true);
        currTime_2->setVisible(true);
        bracketTime->setVisible(true);
        deltaT->setVisible(true);
        if(verticalLine_2->dataMainKey(verticalLine->dataCount()-1) < verticalLine->dataMainKey(verticalLine_2->dataCount()-1))
        {
            bracketTime->right->setCoords(verticalLine_2->dataMainKey(verticalLine_2->dataCount()-1),
                                         verticalLine_2->dataMainValue(verticalLine_2->dataCount()-1));
            bracketTime->left->setCoords(verticalLine->dataMainKey(verticalLine_2->dataCount()-1),
                                          verticalLine->dataMainValue(verticalLine_2->dataCount()-1));

        }
        else
        {
            bracketTime->left->setCoords(verticalLine_2->dataMainKey(verticalLine_2->dataCount()-1),
                                         verticalLine_2->dataMainValue(verticalLine_2->dataCount()-1));
            bracketTime->right->setCoords(verticalLine->dataMainKey(verticalLine_2->dataCount()-1),
                                          verticalLine->dataMainValue(verticalLine_2->dataCount()-1));
        }
        deltaT->setText(QString("%1").arg(fabs(currTime->text().toDouble() - currTime_2->text().toDouble())));
        QPointF point = bracketTime->center->pixelPosition();//->coordsToPixels(x[0],y[0]);
        point.setY(point.y()+10);
//        point.setX(point.x());
        deltaT->position->setPixelPosition(point);

    }
    else
    {
        verticalLine_2->setVisible(false);
        currTime_2->setVisible(false);
        bracketTime->setVisible(false);
        deltaT->setVisible(false);
    }

    ui->customplot->replot();
}
void reportviewQt::slotMousePress(QMouseEvent* event)
{
    //! Обработка события, когда нажата правая кнопка мыши
    if(QApplication::mouseButtons() == Qt::RightButton)
    {
        coordX = ui->customplot->xAxis->pixelToCoord(event->pos().x());

        //
        for(int i = 0; i < listParams.size(); i++)
        {
            if(listParams.at(i)->getGraph()->selected())
            {
                QCPItemTracer* tracer = listParams.at(i)->tracer;
                tracer->setGraphKey(coordX);
                listParams.at(i)->currTracerValue->setText(QString("%1").arg(tracer->position->value()));
                QPointF point = tracer->position->pixelPosition();
                point.setY(point.y()-10);
                point.setX(point.x()+25);
                listParams.at(i)->currTracerValue->position->setPixelPosition(point);
            }
        }
    }
    if(QApplication::mouseButtons() == Qt::RightButton
       && QApplication::keyboardModifiers() == Qt::ControlModifier)
          slotReplotVertLine(verticalLine_2, currTime_2);

    ui->customplot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    //! Если ось x выделена, то разрешено перемещение только по ней,
    //! иначе - перемещение может происходить по обеим осям.
    if (ui->customplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customplot->axisRect()->setRangeDrag(ui->customplot->xAxis->orientation());
    else if(ui->customplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        ui->customplot->axisRect()->setRangeDrag(ui->customplot->yAxis->orientation());
        ui->customplot->axisRect()->setRangeDragAxes(getAllAxis());
    }
    else
    {
        ui->customplot->axisRect()->setRangeDragAxes(getAllAxis());
    }
    //! Если есть выделенные графики, то разрешено только их перемещение по вертикальной оси.
    QList< QCPGraph *> list = getSelectedGraphs();
    if(!list.isEmpty())
    {
        QList< QCPAxis *> list_ax;
        list_ax.clear();
        list_ax.append(ui->customplot->xAxis);
        //list_ax.append(ui->customplot->yAxis);
        for(int i = 0; i < list.count(); i++)
        {
            list_ax.append(list.at(i)->valueAxis());
            list.at(i)->valueAxis()->grid()->setSubGridVisible(true);
        }
        ui->customplot->axisRect()->setRangeDragAxes(list_ax);
        ui->customplot->xAxis->grid()->setSubGridVisible(true);
        for(int i = 0; i < list_ax.count(); i++)
        {
            list_ax.at(i)->grid()->setSubGridVisible(true);
            ui->customplot->xAxis->grid()->setSubGridVisible(true);
        }
    }
    ui->customplot->replot();
}
//! Масштабирование (вращение колесиком мышки)
void reportviewQt::slotMouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event);

    //! Если ось x выделена, то разрешено масштабирование только по ней.
    //! Если ни одна из осей не выделена - масштабирование может происходить по обеим осям.
    ui->customplot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    if(ui->customplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        ui->customplot->axisRect()->setRangeZoom(ui->customplot->xAxis->orientation());
        ui->customplot->axisRect()->setRangeZoomAxes(getAllAxis());
    }
    else if(ui->customplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        ui->customplot->axisRect()->setRangeZoom(ui->customplot->yAxis->orientation());
        ui->customplot->axisRect()->setRangeZoomAxes(getAllAxis());
    }
    else if(!getSelectedGraphs().isEmpty())
    {
        //! Если есть выделенные графики, то разрешено только их масштабирование по вертикальной оси.
        QList< QCPGraph *> list = getSelectedGraphs();
        if(!list.isEmpty())
        {
            QList< QCPAxis *> list_ax;
            list_ax.clear();
            for(int i = 0; i < list.count(); i++)
            {
                list_ax.append(list.at(i)->valueAxis());
                list.at(i)->valueAxis()->grid()->setSubGridVisible(true);
            }
            ui->customplot->axisRect()->setRangeZoomAxes(list_ax);
        }
    }
    else
    {
        ui->customplot->axisRect()->setRangeZoomAxes(getAllAxis());
    }
    ui->customplot->replot();
}

//! Перерисовка положения вертикальной линии.
void reportviewQt::slotReplotVertLine(QCPCurve* vLine, QCPItemText* _currTime)
{
    QVector<double> x(2), y(2);
        x[0] = x[1] = coordX;
        y[0] = ui->customplot->yAxis->range().lower;
        y[1] = ui->customplot->yAxis->range().upper;
    vLine->setData(x, y);
    //! Отображение текущего времени рядом с линией
    _currTime->setText(QString("%1").arg(coordX));
    QPointF point = vLine->coordsToPixels(x[0],y[0]);
    point.setY(point.y()-10);
    point.setX(point.x()+25);
    _currTime->position->setPixelPosition(point);
    ui->customplot->replot();
}

//! Перерисовка списка графиков (левая часть интерфейса)
void reportviewQt::slotReplotListParams()
{
    //Обновление таблицы-списка графиков
    tabWidget->clear();
    tabWidget->setRowCount(listParams.size());
    QWidget *widg[5];
    QHBoxLayout *hlayout[5];
    for(int i = 0; i < listParams.size(); i++)
    {

//        gridLayout_params->addWidget(listParams.at(i)->cbox,        i+1, 0);
//        gridLayout_params->addWidget(listParams.at(i)->label_name,  i+1, 1);
//        gridLayout_params->addWidget(listParams.at(i)->label_color, i+1, 2);
//        gridLayout_params->addWidget(listParams.at(i)->edit_val,    i+1, 3);
//        gridLayout_params->addWidget(listParams.at(i)->label_measure, i+1, 4);

        widg[0] = new QWidget();
        hlayout[0] = new QHBoxLayout(widg[0]);
        hlayout[0]->addWidget(listParams.at(i)->cbox);
        hlayout[0]->setAlignment(Qt::AlignCenter);
        hlayout[0]->setContentsMargins(0,0,0,0);
        tabWidget->setCellWidget(i,0, widg[0]);

        widg[1] = new QWidget();
        hlayout[1] = new QHBoxLayout(widg[1]);
        hlayout[1]->addWidget(listParams.at(i)->label_name);
        hlayout[1]->setAlignment(Qt::AlignLeft);
        hlayout[1]->setContentsMargins(0,0,0,0);
        tabWidget->setCellWidget(i,1, widg[1]);

        widg[2] = new QWidget();
        hlayout[2] = new QHBoxLayout(widg[2]);
        hlayout[2]->addWidget(listParams.at(i)->label_color);
        hlayout[2]->setAlignment(Qt::AlignCenter);
        hlayout[2]->setContentsMargins(0,0,0,0);
        tabWidget->setCellWidget(i,2, widg[2]);

        widg[3] = new QWidget();
        hlayout[3] = new QHBoxLayout(widg[3]);
        hlayout[3]->addWidget(listParams.at(i)->edit_val);
        hlayout[3]->setAlignment(Qt::AlignLeft);
        hlayout[3]->setContentsMargins(0,0,0,0);
        tabWidget->setCellWidget(i,3, widg[3]);

        widg[4] = new QWidget();
        hlayout[4] = new QHBoxLayout(widg[4]);
        hlayout[4]->addWidget(listParams.at(i)->label_measure);
        hlayout[4]->setAlignment(Qt::AlignCenter);
        hlayout[4]->setContentsMargins(0,0,0,0);
        tabWidget->setCellWidget(i,4, widg[4]);

//        listParams.at(i)->edit_val->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    }
    tabWidget->resizeColumnsToContents();

    ui->customplot->replot();
}

void reportviewQt::slotReplotCurrAxes()
{
    //Обновление таблицы-списка графиков приводит к изменению расположения осей вдоль области отображения графиков
    QCPAxis::AxisType ax_type = QCPAxis::atLeft;
    QCPAxisRectPro* rect = (QCPAxisRectPro*)ui->customplot->axisRect();
    int j = 0;
    for(int i = 0; i < listParams.size(); i++)
    {
        if(listParams.at(i)->cbox->isChecked())
        {
            if(j < MAX_NUMBER_OF_AXES)
            {
                rect->removeAxisToOtherSide(listParams.at(i)->getAxis(),ax_type);
                ax_type = ((ax_type == QCPAxis::atLeft) ? QCPAxis::atRight : QCPAxis::atLeft);
                listParams.at(i)->getAxis()->setVisible(true);
            }
            else
            {
                listParams.at(i)->getAxis()->setVisible(false);
            }
            j++;
        }
    }
    ui->customplot->replot();
}

//! Удаление выбранного графика отовсюду
void reportviewQt::slotDeleteFromList(visualParamUnit* vp)
{
    int i = listParams.indexOf(vp);
    if(i > -1)
    {
        ui->customplot->axisRect()->removeAxis(listParams.at(i)->getAxis());
        ui->customplot->removeGraph(listParams.at(i)->getGraph());
        listParams.at(i)->deleteLater();
        listParams.removeAt(i);
        slotReplotListParams();
    }
}

void reportviewQt::slotDeleteFromList(QList<visualParamUnit*> list_vp)
{
    for(int j = 0; j < list_vp.size(); j++)
    {
        int i = listParams.indexOf(list_vp.at(j));
        if(i > -1)
        {
            ui->customplot->axisRect()->removeAxis(listParams.at(i)->getAxis());
            ui->customplot->removeGraph(listParams.at(i)->getGraph());
            listParams.at(i)->deleteLater();
            listParams.removeAt(i);
        }
    }
    listParamNamesFromFile.clear();
    listMeasuresFromFile.clear();
    slotReplotListParams();
}

void reportviewQt::slotAllGraphsVisible()
{
    for(int i = 0; i < listParams.size(); i++)
    {
        if(!listParams.at(i)->cbox->isChecked())
        {
            listParams.at(i)->cbox->click();
            listParams.at(i)->cbox->setChecked(true);
        }
    }
}

void reportviewQt::slotAllGraphsInvisible()
{
    for(int i = 0; i < listParams.size(); i++)
    {
        if(listParams.at(i)->cbox->isChecked())
        {
            listParams.at(i)->cbox->click();
            listParams.at(i)->cbox->setChecked(false);
        }
    }
}

QList< QCPAxis *> reportviewQt::getAllAxis()
{
    QList< QCPAxis *> list;
    list.clear();
    list.append(ui->customplot->xAxis);
    for(int i = 0; i < listParams.size(); i++)
        list.append(listParams[i]->getAxis());
    return list;
}

QList< QCPGraph *> reportviewQt::getSelectedGraphs()
{
    QList< QCPGraph *> list;
    list.clear();
    for(int i = 0; i < listParams.size(); i++)
    {
        if(listParams[i]->getGraph()->selected())
            list.append(listParams[i]->getGraph());
    }
    return list;
}

void reportviewQt::slotBeforeReplot()
{
    // Вертикальная линия
    slotReplotVertLine(verticalLine, currTime);
    // Трассировка
    for (int i = 0; i < listParams.size(); i++)
    {
        if(listParams.at(i)->cbox->isChecked())
        {
            int indx = listParams.at(i)->getGraph()->findBegin(coordX,true);
            double y_graph ;//= listParams.at(i)->getGraph()->data().value->at(indx)->value;
            listParams.at(i)->edit_val->setText(QString("%1").arg(y_graph));//(listParams.at(i)->currTracerValue->text());
        }
        else
        {
            listParams.at(i)->edit_val->setText(QString(""));
        }
    }

    double eps = 1e-5;
    if( ui->customplot->xAxis->range().lower < 0 )
        ui->customplot->xAxis->setRangeLower(-eps);
//    double upper = 10.;//ui->customplot->xAxis->range().lower + eps;
//    for (int i = 0; i < listParams.size(); i++)
//    {
//        double temp =  listParams.at(i)->getGraph()->dataMainKey(listParams.at(i)->getGraph()->dataCount()-1);
//        if(temp > upper)
//            upper = temp;
//    }
//    if( ui->customplot->xAxis->range().upper > upper )
//        ui->customplot->xAxis->setRangeUpper(upper + eps);
}

//void reportviewQt::on_action_write_to_pdf_clicked()
//{
//    //legend
//    ui->customplot->legend->setVisible(true);
//    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
//    ui->customplot->plotLayout()->addElement(1,0,subLayout);
//    subLayout->setMargins(QMargins(ui->customplot->rect().left(),ui->customplot->rect().top(),
//                                   ui->customplot->rect().right(),ui->customplot->rect().bottom()/50));
//    subLayout->addElement(0,0,ui->customplot->legend);
//    ui->customplot->legend->setFillOrder(QCPLegend::foColumnsFirst);
//    ui->customplot->plotLayout()->setRowStretchFactor(1, 0.01);

//    createPdf->show();
//}

void reportviewQt::slotScrollAreaMenu(QPoint pos)
{
    Q_UNUSED(pos);
    if(!listParams.isEmpty() && !previousFilePath.isEmpty())
    {
        QPoint globalPos = scrollArea->mapToGlobal(pos);
        QMenu menu;
        QAction* act1 = menu.addAction("Добавить из открытого файла...");
        QAction * selectedItem = menu.exec(globalPos);
        if(selectedItem)
        {
            if(selectedItem == act1)
            {
                    //addGraph(file);
                    readParamNamesFromFile(previousFilePath);
                    formSelectParamsFromFile->setPath(previousFilePath);
                    formSelectParamsFromFile->setParamNames(listParamNamesFromFile);
                    formSelectParamsFromFile->show();
            }
        }
    }
}

void reportviewQt::slotAboutThis()
{
    QMessageBox msgBox;
//    msgBox.setWindowIcon(windowIcon);
//    msgBox.setIcon(QString);
//    msgBox.setText("Программа просмотра графиков reportViewQt ver 1.1");
//    msgBox.setInformativeText("Собрана 22.04.2017. \n Ревизия ... \n © Компания \"Сухой\" \"ОКБ Сухого\"");
    msgBox.about(this,"О программе \"Просмотр графиков\"...",
                 "Проект reportViewQt ver 1.1. \n\nСобран 22.04.2017. \n\nРевизия: 5f544660a1e137a54d9a006f7efb2141ab179c0a \n\nРазработчик: Микушкина Н.Ю., тел. 67-87\n\n©2017 Компания \"Сухой\" \"ОКБ Сухого\"");
}
