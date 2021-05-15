#ifndef DIAL_H
#define DIAL_H

#include <QtGui>
#include <QWidget>

/*  自定义表盘
 *      内部自动设定组件的数字,指针等大小,可以随意变大变小
 *      内部集成定时器,可以通过setTimerType()实现动画旋转快慢
 *      通过setGradientColorMode()可以自定义渐变颜色,通过setSingleColorMode()自定义单一颜色
 *      支持负数到正数更改
 *      支持小数点位数设置
 *      优化刻度值自适应窗口，能显示更正显示个位至千位
 *      支持表盘所有颜色搭配
 */

class Dial : public QWidget
{
    Q_OBJECT

    enum ModeColor {            //划过的颜色模式

        ModeSingleColor,            //单一颜色
        ModeGradientColor          //渐变颜色
    };


public:

    bool isFlat;                //默认为false,表示表盘拥有阴影,实现立体效果

    int  Scale_decimal;         //保留小数点,默认为保留1个

    int  center_decimal;        //中心圆数值小数点保留,默认为保留1个

    qreal minvalue;             //表盘刻度最小值
    qreal maxvalue;             //表盘刻度最大值
    qreal value;                //当前值
    qreal endvalue;             //需要到达的值

    ModeColor modeColor;        //模式,构造函数下默认为单色模式

    QTimer   timer;             //定时器,用来实现动画旋转指针
    int  timer_msec;             //定时器ms参数,表示每过多少ms中断一次
    int  timer_value;           //定时器值参数,表示每中断一次,则加减多少值(要为正数)


    QString  label;             //标签
    QString  Value_Text;        //值
    QString  iconfile;          //图标文件位置
    QPixmap  icon;              //图标


    int radius;             //仪表盘最外圈半径

    QColor  ScaleColor;         //刻度颜色
    QColor  ValueColor;         //值颜色      110V
    QColor  LabelColor;          //标签颜色   电压表

    QColor  PointerColor;       //指针颜色
    QColor  SingleSlideColor;    //划过的单色

    QList<QColor>   GradientSlideColor; //渐变颜色


    QColor  slideScaleColor;     //划过的刻度颜色



    QColor  bkColor;             //第二个圆背景色


    QColor  obkColor;            //外圆背景色
    QColor  centercolor;        //中心圆盘背景色

    qreal   centerR;          //中心圆盘半径




protected:
    void drawObkColor(QPainter &paint);      //绘制外圆


    void drawScalebkColor(QPainter &paint);       //绘制刻度圆

    void drawbkColor(QPainter &paint);           //绘制内圆(用来遮挡滑动的颜色)

    void drawCenterColor(QPainter &paint);      //绘制中心圆

    void drawScaleColor(QPainter &paint);       //绘制刻度

    void drawScaleTextColor(QPainter &paint);    //绘制刻度值

    void drawShade(QPainter &paint);              //绘制阴影


    void drawslideScaleColor(QPainter &paint);     //绘制划过的颜色

    void drawslideScaleGradientColor(QPainter &paint);     //绘制划过的渐变色
    void drawslideScaleSingleColor(QPainter &paint);     //绘制划过的单色

    void drawPointColor(QPainter &paint);                //绘制指针

    void drawIconValueColor(QPainter &paint);            //绘制图标和value

    void drawlabelColor(QPainter &paint);               //绘制标签

    void drawDialShade(QPainter &paint,qreal Angle_start,qreal Angle_end,qreal ratio,int alpha);
                                                        //给整个表盘绘制阴影
    // Angle_start Angle_end :表示圆盘的起始/结束角度,Angle_end必须大于Angle_start哦
    //ratio:表示绘制的阴影亮度的扁与平(为0~1之间),为0表示平的圆环,为0.99表示最扁的圆环,大于等于1则不会绘制
    //alpha:表示铺上的白色透明度


    void paintEvent(QPaintEvent *);

    QPoint CustomRotate(QPointF point,qreal from_angle,qreal rotate);

    QString  fetchDecimalPoint(QString value,int point);   //value:值  point:要保留的小数位
     //通过给定的数,来获取要保留的数,比如:参数为12.234,由于decimal_point=1,所以返回值为12.3




protected slots:
    void ontimeout();


public:
    Dial(QWidget *parent = 0,QString labelText="",QString valueText="",QString iconFile="" );

    /*设置圆组件颜色*/
    void setobkColor(QColor obkColor);
    void setbkColor(QColor bkColor);
    void setcentercolor(QColor centercolor);


    /*设置指针组件颜色*/
    void setPointerColor(QColor PointerColor);

    /*设置文字颜色(刻度颜色,划过的刻度颜色,中心值颜色,标签值颜色)*/
    void setScaleColor(QColor ScaleColor);
    void setslideScaleColor(QColor slideScaleColor);
    void setValueColor(QColor ValueColor);
    void setLabelColor(QColor LabelColor);


    void setFlat(bool isFlat);              //设置表盘是否是平面的

    void setGradientColorMode(QList<QColor>& Qcolors);       //添加渐变
    void setSingleColorMode(QColor color);


    bool set_Scale_decimal(int point);          //设置刻度值保留小数多少位
    bool set_Center_decimal(int point);          //设置中心数值保留小数多少位

    void setTimerType(int msec,int v);  //设置定时器参数,每过多少ms,跑多少值,如果ms=0或者v=0,则表示不用定时器,直接跳到终点






    qreal EndValue();                   //读取终点值
    qreal CurrentValue();               //读取当前值
    qreal MinValue();                   //读取最小值
    qreal MaxValue();                   //读取最大值


    bool setCurrentValue(qreal v);      //设置当前值(同时也会将终点值设为一样)

    bool setEndValue(qreal v);          //设置终点值

    bool setMinValue(qreal v);    //设置最小值
    bool setMaxValue(qreal v);    //设置最大值

    bool setValues(qreal cV,qreal minV,qreal maxV,qreal endV);
                                        //设置所有值:当前值,最小值,最大值,终点值



signals:
    void arriveEnd(qreal endvalue);
    void currentValueChange(qreal CurrentValue);
    void arriveEnd();

public:
    void sendEnd()
    {
        emit arriveEnd(this->endvalue);
        emit arriveEnd();
    }

    void sendChangeValue()
    {
        emit currentValueChange(this->value);
    }

public slots:

};

#endif // DIAL_H
