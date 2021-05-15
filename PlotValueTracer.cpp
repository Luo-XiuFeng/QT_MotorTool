#include "PlotValueTracer.h"

PlotValueTracer::PlotValueTracer(QPointer<QCustomPlot> _parentPlot,PlotValueTracerType _type,QCPGraph *_graph) :
    QObject(_parentPlot),
    m_parentPlot(_parentPlot),
    m_type(_type),
    m_graph(_graph),
    m_nLineLength(15)
{
    //游标点
    m_tracer = new QCPItemTracer(m_parentPlot);
    if(m_graph)
    {
        m_tracer->setPen(m_graph->pen());
        m_tracer->setBrush(m_graph->brush());
    }
    //线元素
    m_arrowLine = new QCPItemLine(m_parentPlot);
    m_arrowLine->setLayer("overlay");
    m_arrowLine->setClipToAxisRect(false);
    if(m_graph)
        m_arrowLine->setPen(m_graph->pen());//设置箭头的颜色
    m_arrowLine->setHead(QCPLineEnding::esSpikeArrow);

    //文本元素
    m_labelText = new QCPItemText(m_parentPlot);
    m_labelText->setLayer("overlay");
    m_labelText->setClipToAxisRect(false);
    m_labelText->setPadding(QMargins(3, 4, 3, 4));
    if(m_graph)
        m_labelText->setPen(m_graph->pen());
    m_labelText->position->setParentAnchor(m_tracer->position);//m_arrowLine->start);

    switch(m_type)
    {
    case XAxisTracer:
        m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
        m_tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);

        m_arrowLine->end->setParentAnchor(m_tracer->position);
        m_arrowLine->start->setParentAnchor(m_arrowLine->end);
        m_arrowLine->start->setCoords(m_nLineLength, 0);//偏移量，用于设置线元素的长度

        m_labelText->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        break;
    case YAxisTracer:
        m_tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
        m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

        m_tracer->position->setCoords(1, 0);  //这一句必不可少

        m_arrowLine->end->setParentAnchor(m_tracer->position);
        m_arrowLine->start->setParentAnchor(m_arrowLine->end);//m_labelText->position  m_arrowLine->end
        m_arrowLine->start->setCoords(m_nLineLength, 0);//偏移量，用于设置线元素的长度

        m_labelText->setPositionAlignment(Qt::AlignTop|Qt::AlignVCenter);
        break;
    case DataTracer:
        m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
        m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

        m_arrowLine->end->setParentAnchor(m_tracer->position);
        m_arrowLine->start->setParentAnchor(m_arrowLine->end);
        m_arrowLine->start->setCoords(m_nLineLength, 0);//偏移量，用于设置线元素的长度

        m_labelText->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        break;
    }


    setVisible(false);


}
//析构函数
PlotValueTracer::~PlotValueTracer()
{
    if (m_tracer)
        m_tracer->parentPlot()->removeItem(m_tracer);
    if (m_arrowLine)
        m_arrowLine->parentPlot()->removeItem(m_arrowLine);
    if (m_labelText)
        m_labelText->parentPlot()->removeItem(m_labelText);
}
//设置游标点的样式
void PlotValueTracer::setTracerStyle(QCPItemTracer::TracerStyle e)
{
    if(m_tracer)
        m_tracer->setStyle(e);
}
//设置线元素的偏移长度
void PlotValueTracer::setArrowLineLength(int len)
{
    m_nLineLength = len;
    m_arrowLine->start->setCoords(m_nLineLength, 0);//偏移量
}
//设置线元素头样式
void PlotValueTracer::setArrowHead(QCPLineEnding::EndingStyle e)
{
    if(m_arrowLine)
        m_arrowLine->setHead(e);
}
//设置线元素的画笔
void PlotValueTracer::setArrowPen(const QPen &pen)
{
    if(m_arrowLine)
        m_arrowLine->setPen(pen);
}
//设置游标点的画笔
void PlotValueTracer::setTracerPen(const QPen &pen)
{
    if(m_tracer)
        m_tracer->setPen(pen);
}
//设置文本元素的画笔
void PlotValueTracer::setLabelPen(const QPen &pen)
{
    if(m_labelText)
        m_labelText->setPen(pen);
}
//设置游标点的画刷
void PlotValueTracer::setTracerBrush(const QBrush &brush)
{
    if(m_tracer)
        m_tracer->setBrush(brush);
}
//设置文本元素画刷
void PlotValueTracer::setLabelBrush(const QBrush &brush)
{
    if(m_labelText)
        m_labelText->setBrush(brush);
}
//设置文本元素文本
void PlotValueTracer::setText(const QString &text)
{
    if(m_labelText)
        m_labelText->setText(text);
}
//设置文本元素中文本的颜色
void PlotValueTracer::setTextColor(QColor &c)
{
    if(m_labelText)
        m_labelText->setColor(c);
}
//设置文本元素边界
void PlotValueTracer::setTextMargin(QMargins mar)
{
    if(m_labelText)
        m_labelText->setPadding(mar);
}
//设置游标的显示/隐藏
void PlotValueTracer::setVisible(bool visible)
{
    if(m_tracer)
        m_tracer->setVisible(visible);

    if(m_arrowLine)
        m_arrowLine->setVisible(visible);

    if(m_labelText)
        m_labelText->setVisible(visible);
}
//更新游标位置
void PlotValueTracer::updateTracerPosition(double x, double y)
{
    switch(m_type)
    {
    case XAxisTracer:
        updatePositionX(x);
        break;
    case YAxisTracer:
        updatePositionY(y);
        break;
    case DataTracer:
        updatePositionData(x,y);
        break;
    }
}
//XAxisTracer显示模式，更新游标位置-UNTEST
void PlotValueTracer::updatePositionX(double xValue)
{
    setVisible(true);
    m_tracer->position->setCoords(xValue, 1);
    m_labelText->position->setCoords(0, m_nLineLength);
    m_arrowLine->start->setCoords(0, m_nLineLength);
    m_arrowLine->end->setCoords(0, 0);
}
//YAxisTracer显示模式，更新游标位置
void PlotValueTracer::updatePositionY(double value)
{
    setVisible(true);
    // since both the arrow and the text label are chained to the dummy tracer (via anchor
    // parent-child relationships) it is sufficient to update the dummy tracer coordinates. The
    // Horizontal coordinate type was set to ptAxisRectRatio so to keep it aligned at the right side
    // of the axis rect, it is always kept at 1. The vertical coordinate type was set to
    // ptPlotCoordinates of the passed parent axis, so the vertical coordinate is set to the new
    // value.
    m_tracer->position->setCoords(1, value);

    // We want the arrow head to be at the same horizontal position as the axis backbone, even if
    // the axis has a certain offset from the axis rect border (like the added second y axis). Thus we
    // set the horizontal pixel position of the arrow end (head) to the axis offset (the pixel
    // distance to the axis rect border). This works because the parent anchor of the arrow end is
    // the dummy tracer, which, as described earlier, is tied to the right axis rect border.
    //m_arrowLine->end->setCoords(m_axis->offset(), 0);
    m_labelText->position->setCoords(10, 0);
}

//DataTracer显示模式，更新游标位置
void PlotValueTracer::updatePositionData(double xValue, double yValue)
{
    setVisible(true);
    if (yValue > m_parentPlot->yAxis->range().upper)
        yValue = m_parentPlot->yAxis->range().upper;

    m_tracer->position->setCoords(xValue, yValue);
    m_labelText->position->setCoords(m_nLineLength, 0);
}
