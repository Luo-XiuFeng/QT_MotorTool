#ifndef PLOTVALUETRACER_H
#define PLOTVALUETRACER_H

#include <QObject>
#include "qcustomplot.h"

//定义追踪样式
enum PlotValueTracerType
{
    XAxisTracer,
    YAxisTracer,
    DataTracer
};

class PlotValueTracer : public QObject
{
    Q_OBJECT
public:

    explicit PlotValueTracer(QPointer<QCustomPlot> _parentPlot, PlotValueTracerType _type,QCPGraph *_graph = 0);
    ~PlotValueTracer();

    // setters:
    void setTracerStyle(QCPItemTracer::TracerStyle e); //设置游标点的样式
    void setArrowLineLength(int len);                  //设置线元素的偏移长度
    void setArrowHead(QCPLineEnding::EndingStyle e);   //设置线元素头样式

    void setArrowPen(const QPen &pen);                 //设置线元素的画笔
    void setTracerPen(const QPen &pen);                //设置游标点的画笔
    void setLabelPen(const QPen &pen);                 //设置文本元素的画笔

    void setTracerBrush(const QBrush &brush);          //设置游标点的画刷
    void setLabelBrush(const QBrush &brush);           //设置文本元素画刷

    void setText(const QString &text);                 //设置文本元素文本
    void setTextColor(QColor &c);                      //设置文本元素中文本的颜色
    void setTextMargin(QMargins mar);                  //设置文本元素边界


    void setVisible(bool visible);                     //设置游标的显示/隐藏
    void updateTracerPosition(double x,double y);      //更新游标位置
private:
    void updatePositionX(double value);                      //XAxisTracer显示模式，更新游标位置
    void updatePositionY(double value);                      //YAxisTracer显示模式，更新游标位置
    void updatePositionData(double xValue, double yValue);   //DataTracer显示模式，更新游标位置
private:
    QPointer<QCustomPlot> m_parentPlot;  //曲线图
    QCPGraph *m_graph;	   	             //这里是存传入的绘图图层
    QPointer<QCPItemTracer> m_tracer;    //数据跟踪点
    QPointer<QCPItemLine> m_arrowLine;   //线元素
    QPointer<QCPItemText> m_labelText;   //数据跟踪点文本元素
    PlotValueTracerType m_type;          //追踪点类型
    int m_nLineLength;                   //线元素的长度
};

#endif // PLOTVALUETRACER_H
