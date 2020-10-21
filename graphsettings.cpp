#include "graphsettings.h"
#include "ui_graphsettings.h"

QList<graphsettings*>* graphsettings::listOfSettings = 0;

graphsettings::graphsettings(QWidget *parent, QColor _col) :
    QWidget(parent),
    ui(new Ui::graphsettings)
{
    ui->setupUi(this);
    setWindowTitle(QString("Настройки графика..."));

    isGeneralSettings = false;//по умолчанию - настройки для конкретного графика
    if(listOfSettings == 0)
        listOfSettings = new QList<graphsettings*>;

    path = "no_path";
    activeState.curr_col = _col;
    activeState.paramname = "";
    activeState.pointtype = 0;
    //Оптимизация
    activeState.isOptimize = 0;
    activeState.isOptimize_prev = activeState.isOptimize;
    activeState.isDeltaActive = false;
    activeState.deltaOptimize = 1e-10;
    activeState.deltaOptimize_prev = activeState.deltaOptimize;
    workState = activeState;

    //! Выбор типа линии
    setGraphPointTypes();

    //! M E N U
    //! the 1st
    QIcon ic_common = QIcon(":/folder");
    ui->listWidget->addItem(new QListWidgetItem(ic_common, QString("Общая информация")));
    //! the 2st
    QIcon icon_color;
    icon_color.addFile(":/color_fonts_dialog");
    QListWidgetItem* item = new QListWidgetItem(icon_color, QString("Стиль графика"));
    ui->listWidget->addItem(item);
          // пока не реализовано!
          ui->comboBox_linetype->setVisible(false);
          ui->label_linetype->setVisible(false);
          ui->label_pointcolor->setVisible(false);
          ui->label_col_pointcolor->setVisible(false);
          ui->pushButton__pointcolor->setVisible(false);
          ui->label_pointsize->setVisible(false);
          ui->spinBox_pointsize->setVisible(false);
    //! the 3rd
    QIcon ic_2 = QIcon(":/QCPGraph");
    ui->listWidget->addItem(new QListWidgetItem(ic_2, QString("Данные")));
    //! COMMON
    ui->listWidget->setIconSize(QSize(40,40));
    //! Выбор по умолчанию
    ui->listWidget->setItemSelected(ui->listWidget->item(0),true);
    slotItemClicked(ui->listWidget->item(0));

    ui->lineEdit_path->setReadOnly(true);

    ui->radioButton_no_optim->setChecked(!activeState.isOptimize);
    ui->checkBox->setEnabled(activeState.isDeltaActive);
    ui->label_cbox->setEnabled(false);
    ui->lineEdit_cbox->setEnabled(false);
    ui->lineEdit_cbox->setText(QString("%1").arg(activeState.deltaOptimize));

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(ui->pushButton_OK, SIGNAL(clicked()), this, SLOT(slotOK()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(slotApply()));
    //! 1
    connect(ui->lineEdit_param_name, SIGNAL(textChanged(QString)), this, SLOT(slotParamNameChanged(QString)));
    //! 2
    connect(ui->pushButton_col, SIGNAL(clicked()), this, SLOT(slotColorDialog()));
    connect(ui->comboBox_pointtype, SIGNAL(activated(int)), this, SLOT(slotComboBoxActivated(int)));
    connect(this, SIGNAL(signalChangeColorGraph(QColor)), this, SLOT(setColor(QColor)));
    //! 3
    connect(ui->radioButton_no_optim, SIGNAL(clicked()), this, SLOT(slotRadioButton()));
    connect(ui->radioButton_optim, SIGNAL(clicked()), this, SLOT(slotRadioButton()));
    connect(ui->lineEdit_cbox, SIGNAL(editingFinished()), this, SLOT(slotLineEditCbox()));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(slotCheckBox(bool)));
}

graphsettings::~graphsettings()
{
    delete ui;
}
//! Инициализация параметров
void graphsettings::initial(QColor _col, int _pointtype, QString _paramname, QString _path)
{
    activeState.curr_col = _col;
    activeState.pointtype = _pointtype;
    activeState.paramname = _paramname;
    path = _path;
    setState(activeState);
    workState = activeState;    

    if(!listOfSettings->contains(this))
        listOfSettings->append(this);
}
//! Инициализация для общих настроек (активна только оптимизация всех графиков)
void graphsettings::initialGeneralSettings()
{
    isGeneralSettings = true;
    ui->listWidget->setItemSelected(ui->listWidget->item(2),true);
    ui->stackedWidget->widget(0)->setEnabled(false);
    ui->stackedWidget->widget(1)->setEnabled(false);
    slotItemClicked(ui->listWidget->item(2));
}

//! Заполняем список возможных стилей точек графика
void graphsettings::setGraphPointTypes()
{
    ui->comboBox_pointtype->resize(190, 22);
    QList<QIcon> icons;
    QList<QString> names;
    icons.append(QIcon(""));
    names.append(QString("Не выбран"));
    pointtypes.append(QString(""));

    icons.append(QIcon(":/ssDot"));
    names.append(QString("Точка"));
    pointtypes.append(QString("ssDot"));

    icons.append(QIcon(":/ssCross"));
    names.append(QString("Крест"));
    pointtypes.append(QString("ssCross"));

    icons.append(QIcon(":/ssPlus"));
    names.append(QString("Плюс"));
    pointtypes.append(QString("ssPlus"));

    icons.append(QIcon(":/ssCircle"));
    names.append(QString("Круг"));
    pointtypes.append(QString("ssCircle"));

    icons.append(QIcon(":/ssDisc"));
    names.append(QString("Закрашенный круг"));
    pointtypes.append(QString("ssDisc"));

    icons.append(QIcon(":/ssSquare"));
    names.append(QString("Квадрат"));
    pointtypes.append(QString("ssSquare"));

    icons.append(QIcon(":/ssDiamond"));
    names.append(QString("Ромб"));
    pointtypes.append(QString("ssDiamond"));

    icons.append(QIcon(":/ssStar"));
    names.append(QString("Звезда"));
    pointtypes.append(QString("ssStar"));

    icons.append(QIcon(":/ssTriangle"));
    names.append(QString("Треугольник"));
    pointtypes.append(QString("ssTriangle"));

    icons.append(QIcon(":/ssTriangleInverted"));
    names.append(QString("Набла"));
    pointtypes.append(QString("ssTriangleInverted"));

    icons.append(QIcon(":/ssCrossSquare"));
    names.append(QString("Квадрат с крестом внутри"));
    pointtypes.append(QString("ssCrossSquare"));

    icons.append(QIcon(":/ssPlusSquare"));
    names.append(QString("Квадрат с плюсом внутри"));
    pointtypes.append(QString("ssPlusSquare"));

    icons.append(QIcon(":/ssCrossCircle"));
    names.append(QString("Круг с крестом внутри"));
    pointtypes.append(QString("ssCrossCircle"));

    icons.append(QIcon(":/ssPlusCircle"));
    names.append(QString("Круг с плюсом внутри"));
    pointtypes.append(QString("ssPlusCircle"));

    icons.append(QIcon(":/ssPeace"));
    names.append(QString("Круг со знаком Pease"));
    pointtypes.append(QString("ssPeace"));
    for(int i = 0; i < icons.size(); i++)
        ui->comboBox_pointtype->addItem(icons.at(i), names.at(i));
}
//! Закрашиваем область текущего цвета в данный, но пока не применяем к графику
void graphsettings::setColor(QColor col)
{
    QPen pen(col);
    int col_r,col_g,col_b; pen.color().getRgb(&col_r,&col_g,&col_b);
    QFont fnt1;
    fnt1.setBold(true);
    ui->label_col->setStyleSheet(QString("background: rgb(%1, %2, %3)").arg(col_r).arg(col_g).arg(col_b));
    ui->label_col->setFont(fnt1);
}
//! Формируем внешний вид формы в соответствии со state
void graphsettings::setState(TStateSettings state)
{
    ui->lineEdit_param_name->setText(state.paramname);
    ui->lineEdit_path->setText(path);
    //! Закрашиваем область текущего цвета в данный, но пока не применяем к графику
    setColor(state.curr_col);
    //! Выставляем значение стиля точек
    ui->comboBox_pointtype->setCurrentIndex(state.pointtype);
    //! Выставляем параметры оптимизации
    ui->radioButton_optim->setChecked(state.isOptimize);
    ui->radioButton_no_optim->setChecked(!state.isOptimize);
    slotRadioButton();
    ui->checkBox->setChecked(state.isDeltaActive);
}

//! Нажатие на кнопку "Принять"
void graphsettings::slotApply()
{
    //цвет и стиль
    setState(workState);
    emit signalParamNameChanged(workState.paramname);
    emit signalChangeColorGraph(workState.curr_col);
    emit signalChanePointTypeGraph(workState.pointtype);
    //изменение параметров оптимизации
    if(workState.isOptimize != workState.isOptimize_prev || workState.deltaOptimize != workState.deltaOptimize_prev)
    {
        if(!isGeneralSettings)
            emit signalOptimizeChanged(workState.isOptimize, workState.deltaOptimize);
        else
        {
            for(int i = 0; i < listOfSettings->size(); i++)
                emit listOfSettings->at(i)->signalOptimizeChanged(workState.isOptimize, workState.deltaOptimize);
        }
    }
    workState.isOptimize_prev = workState.isOptimize;
    workState.deltaOptimize_prev = workState.deltaOptimize;

    activeState = workState;
}
//! Нажатие на кнопку "ОК"
void graphsettings::slotOK()
{
    slotApply();
    close();
}
//! Нажатие на кнопку "Закрыть"
void graphsettings::slotClose()
{
    setState(activeState);

    workState = activeState;
    close();
}
//! Чтение пункта главного меню
void graphsettings::slotItemClicked(QListWidgetItem* item)
{
    if(!item->text().isEmpty())
    {
        ui->label_Name->setText(item->text());
        ui->stackedWidget->setCurrentIndex(ui->listWidget->row(item));
    }
}
//! Изменение названия параметра
void graphsettings::slotParamNameChanged(QString str)
{
    workState.paramname = str;
}
//! Вызов диалогового окна для выбора цвета извне (по нажатие на label_color)
void graphsettings::slotColorDialog(bool b)
{
    Q_UNUSED(b)

    QColorDialog dialog;
    QColor new_col = dialog.getColor(activeState.curr_col);
    if(new_col.isValid())
    {
        emit signalChangeColorGraph(new_col);
        setColor(new_col);
        activeState.curr_col = new_col;
    }
}
//! Вызов диалогового окна для выбора цвета
void graphsettings::slotColorDialog()
{
    QColorDialog dialog;
    QColor new_col = dialog.getColor(workState.curr_col);
    if(new_col.isValid())
    {
        setColor(new_col);
        workState.curr_col = new_col;
    }
}
//! Выбор стиля точек графика
void graphsettings::slotComboBoxActivated(int pointtype)
{
    workState.pointtype = pointtype;
}
//! Вкл./откл. оптимизации данных
void graphsettings::slotRadioButton()
{
    if(ui->radioButton_no_optim->isChecked())
    {
        workState.isOptimize = 0;
        ui->checkBox->setChecked(false);
        ui->checkBox->setEnabled(false);
        ui->label_cbox->setEnabled(false);
    }
    else//if(ui->radioButton_optim->isChecked())
    {
        workState.isOptimize = 1;
        ui->checkBox->setEnabled(true);
        ui->label_cbox->setEnabled(true);
    }
}
//! Сделать активным/неактивным ограничение
void graphsettings::slotCheckBox(bool b)
{
    if(b == true)
    {
        ui->lineEdit_cbox->setEnabled(true);
        ui->lineEdit_cbox->setFocus();
    }
    else
    {
        ui->lineEdit_cbox->setEnabled(false);
    }
    workState.isDeltaActive = b;
}
//! Прочитать значение радиуса окрестности
void graphsettings::slotLineEditCbox()
{
    bool ok;
    double d = ui->lineEdit_cbox->text().toDouble(&ok);
    if(!ok)
    {
        ui->lineEdit_cbox->setText(QString("1e-10"));
    }
    else if(d < 0)
        d *= -1.;
    workState.deltaOptimize_prev = workState.deltaOptimize;
    workState.deltaOptimize = d;
    ui->radioButton_optim->setFocus();
}
