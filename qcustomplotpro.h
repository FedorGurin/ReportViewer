#ifndef QCUSTOMPLOTPRO_H
#define QCUSTOMPLOTPRO_H

#include "qcustomplot.h"

class QCPAxisPro : public QCPAxis
{
    Q_OBJECT
public:
    QCPAxisPro(QCPAxisRect *parent, AxisType type):QCPAxis(parent, type){}
    virtual ~QCPAxisPro(){}

    void setAxisType(AxisType ax_type)
    {
       mAxisType = ax_type;
       mOrientation = orientation(ax_type);
    }
};

class QCPAxisRectPro : QCPAxisRect
{
    Q_OBJECT
public:
    explicit QCPAxisRectPro(QCustomPlot *parentPlot, bool setupDefaultAxes=true):QCPAxisRect(parentPlot, setupDefaultAxes){}
    virtual ~QCPAxisRectPro(){}

    bool removeAxisToOtherSide(QCPAxisPro *axis, QCPAxis::AxisType newType)
    {
        QCPAxis::AxisType type = QCPAxis::atLeft;//default
        //Удаление
      QHashIterator<QCPAxis::AxisType, QList<QCPAxis*> > it(mAxes);
      while (it.hasNext())
      {
        it.next();
        if (it.value().contains((QCPAxis*)axis))
        {
          mAxes[it.key()].removeOne((QCPAxis*)axis);
//          if (qobject_cast<QCustomPlot*>(parentPlot()))
//            parentPlot()->axisRemoved((QCPAxis*)axis);
          break;
        }
      }

      //Добавление
      type = newType;
      axis->setAxisType(type);
      mAxes[type].append(axis);

      // reset convenience axis pointers on parent QCustomPlot if they are unset:
      if (mParentPlot && mParentPlot->axisRectCount() > 0 && mParentPlot->axisRect(0) == this)
      {
        switch (type)
        {
          case QCPAxis::atBottom: { if (!mParentPlot->xAxis) mParentPlot->xAxis = axis; break; }
          case QCPAxis::atLeft: { if (!mParentPlot->yAxis) mParentPlot->yAxis = axis; break; }
          case QCPAxis::atTop: { if (!mParentPlot->xAxis2) mParentPlot->xAxis2 = axis; break; }
          case QCPAxis::atRight: { if (!mParentPlot->yAxis2) mParentPlot->yAxis2 = axis; break; }
        }
      }
      return true;
    }
};

class QCustomPlotPro : public QCustomPlot
{
    Q_OBJECT
public:
    QCustomPlotPro(QWidget *parent = 0):QCustomPlot(parent){}

    QCPAxisRectPro* axisRectPro;

    virtual ~QCustomPlotPro(){}

    void getAxisRect()
    {
//        axisRect()->axis()
    }
};

#endif // QCUSTOMPLOTPRO_H
