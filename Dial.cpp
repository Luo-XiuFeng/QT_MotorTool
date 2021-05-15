#include "Dial.h"
#include <QDebug>
Dial:: Dial(QWidget *parent , QString labelText, QString valueText,  QString iconFile ):
    QWidget(parent),
    label(labelText),
    Value_Text(valueText),
    iconfile(iconFile),
    icon(iconFile),
    timer_msec(0)
{

     Scale_decimal=1;                   //设置保留小数点
     center_decimal=1;

     isFlat = false;


     if(!Value_Text.contains("%1"))
     {
         Value_Text = "%1";
     }

     radius=0;                          //仪表盘最外圈半径

     minvalue=0;
     maxvalue=150;                     //表盘刻度最大值
     value=0;                         //当前值
     endvalue=0;



     modeColor=ModeSingleColor;

     ScaleColor=QColor(185,185,185);            //表盘刻度颜色
     PointerColor=QColor(249,2,1);              //指针颜色
     SingleSlideColor =QColor(83,213,251);      //划过的单颜色
     slideScaleColor = QColor(255,255,255);     //划过的刻度颜色

     ValueColor=QColor(255,255,255);            //值颜色(包括标签)
     LabelColor=QColor(192,192,192);


     obkColor=QColor(252,252,248);               //外圆背景色

     bkColor=QColor(26,26,26);                   //第二个圆背景色

     centercolor=QColor(38,38,38);                //中心圆盘背景色


     connect(&timer,SIGNAL(timeout()),this,SLOT(ontimeout()));
}


void Dial::drawObkColor(QPainter& paint)      //绘制外圆
{
        paint.save();

        QConicalGradient  Conical(0,0,90);


        Conical.setColorAt(0,obkColor);
        Conical.setColorAt(0.5,obkColor);

        Conical.setColorAt(0.12,obkColor.darker(80));
        Conical.setColorAt(0.88,obkColor.darker(80));

        Conical.setColorAt(0.4,obkColor.darker(50));
        Conical.setColorAt(0.6,obkColor.darker(50));


        Conical.setColorAt(0.25,obkColor.darker(130));

        Conical.setColorAt(0.75,obkColor.darker(130));
        Conical.setColorAt(1,obkColor);
        paint.setBrush(Conical);
        paint.drawEllipse(QPointF(0,0), radius*0.96,radius*0.98);

        Conical.setAngle(45);

        Conical.setColorAt(0,obkColor.darker(130));
        Conical.setColorAt(0.5,obkColor.darker(130));

        Conical.setColorAt(0.12,obkColor.darker(80));
        Conical.setColorAt(0.88,obkColor.darker(80));

        Conical.setColorAt(0.4,obkColor.darker(50).darker(130));
        Conical.setColorAt(0.6,obkColor.darker(50));


        Conical.setColorAt(0.25,obkColor.darker(130));

        Conical.setColorAt(0.75,obkColor.darker(130));
        Conical.setColorAt(1,obkColor.darker(130));
//        Conical.setColorAt(0,obkColor.darker(60));
//        Conical.setColorAt(0.5,obkColor.darker(60));

//        Conical.setColorAt(0.25,obkColor.darker(130));

//        Conical.setColorAt(0.75,obkColor.darker(130));

        paint.setBrush(Conical);
        paint.drawEllipse(QPointF(0,0), radius*0.93,radius*0.94);

        paint.restore();
}
void Dial::drawScalebkColor(QPainter &paint)        //绘制刻度圆
{
    paint.save();

    paint.setBrush(bkColor);
    paint.drawEllipse(QPointF(0,0), radius*0.90,radius*0.90);

    paint.restore();
}

void Dial::drawbkColor(QPainter &paint)       //绘制内圆
{
    paint.save();


    paint.setBrush(bkColor);
    paint.drawEllipse(QPointF(0,0), radius*0.77-2,radius*0.77-2);

    paint.setBrush(bkColor.darker(200));

    QRadialGradient Radial(0,0,centerR*1.06,0,0);

    Radial.setColorAt(1,bkColor);
    Radial.setColorAt(0.98,bkColor.darker(100));
    Radial.setColorAt(0.95,bkColor.darker(110));
    Radial.setColorAt(0.90,bkColor.darker(120));

    paint.setBrush(Radial);

    paint.drawEllipse(QPointF(0,0),centerR*1.18,centerR*1.18);

    paint.restore();
}


void Dial::drawCenterColor(QPainter &paint)      //绘制中心圆
{
    paint.save();

    QRadialGradient Radial(0,0,centerR,0,0);

    Radial.setColorAt(1,centercolor.lighter(130));
    Radial.setColorAt(0.96,centercolor.lighter(110));
    Radial.setColorAt(0.95,centercolor.lighter(105));
    Radial.setColorAt(0.80,centercolor);

    paint.setBrush(Radial);
    paint.drawEllipse(QPointF(0,0), centerR,centerR);

    paint.restore();

}

void Dial::drawScaleColor(QPainter &paint)   //绘制刻度
{
    #define  SmallTop        3
    #define  SmallBottom     2

    #define  BigTop          5
    #define  BigBottom       3

    //小刻度(顶部宽度为3,底部为2)
    const QPointF smallPoint[4] = {
        QPointF( - SmallTop / 2, radius*0.90),
        QPointF( SmallTop / 2, radius*0.90),
        QPointF(SmallBottom / 2, radius*0.81),
        QPointF(-SmallBottom / 2, radius*0.81)
    };


    //大刻度(顶部宽度为4,底部为3)
    const QPointF BigPoint[4] = {
        QPointF(- BigTop / 2, radius*0.90),
        QPointF(BigTop / 2, radius*0.90),
        QPointF(BigBottom / 2, radius*0.77),
        QPointF(-BigBottom / 2, radius*0.77)
    };




    paint.save();

    paint.rotate(60);            //旋转到起点=210°



    for (int scale = 0; scale <= 24; ++scale)   //210°到-30°,画7个大刻度
    {
        qreal Current_Value =(qreal)scale*((maxvalue-minvalue)/24);

        if((Current_Value>(value-minvalue)&&(paint.brush().color()!=ScaleColor)))
        {
            paint.setBrush(ScaleColor);

        }
        else if((Current_Value<=(value-minvalue)&&(paint.brush().color()!=slideScaleColor)))
        {
            paint.setBrush(slideScaleColor);
        }

      if(scale%4==0)
      {
       paint.drawConvexPolygon(BigPoint, 4);
      }
      else
        paint.drawConvexPolygon(smallPoint, 4);

      paint.rotate(10);
    }

     paint.restore();



}

void Dial::drawScaleTextColor(QPainter &paint)    //绘制刻度值
{

    int alingns[7]={

        Qt::AlignVCenter|Qt::AlignLeft,
        Qt::AlignLeft|Qt::AlignHCenter,
        Qt::AlignLeft|Qt::AlignTop,
        Qt::AlignTop|Qt::AlignHCenter,
        Qt::AlignRight|Qt::AlignTop,
        Qt::AlignRight|Qt::AlignHCenter,
        Qt::AlignBottom|Qt::AlignRight
    };
        QPoint offest[7]={        /*校正文字位置*/
            QPoint(1,-3),
            QPoint(-4,0),
            QPoint(0,8),
            QPoint(0,0),
            QPoint(-3,8),
            QPoint(-3,0),
            QPoint(-3,0)
         };

    int size;           //动态计算文字大小

    if(radius<=120)
        size = 10;
    else if((radius>120)&&(radius<500))
        size = 13+(radius-120)/30;
    else if(radius>=500)
        size = 13+(radius-120)/40;

    if((maxvalue-minvalue)>500)
    {
       qreal ratio= (maxvalue-minvalue)/5000.0;

       size = size/(1+ratio*0.1 +0.1);
    }

        paint.save();

        paint.setPen(ScaleColor);
        QString text("%1");


        QFont   font;

        font.setFamily("Euphemia");
        font.setPixelSize(size*90/72);
        font.setWeight(QFont::DemiBold);
        paint.setFont(font);



        QPoint TextPoint(0,radius*0.77);               //设置90°的文字
        TextPoint = CustomRotate(TextPoint,90,240);   //获取点=210°的文字位置
        qreal TextRotate=210;

         for(int i=0;i<7;i++)              //设置7个点
        {

          qreal Current_Value =(qreal)i*((maxvalue-minvalue)/6)+minvalue;

          if((Current_Value>(value)&&(paint.pen().color()!=ScaleColor)))
          {
              paint.setPen(ScaleColor);

          }
          else if((Current_Value<=(value)&&(paint.pen().color()!=slideScaleColor)))
          {
              paint.setPen(slideScaleColor);
          }

           QString values= fetchDecimalPoint(text.arg(Current_Value),Scale_decimal);  //获取小数点允许范围的值

           qreal xoffset;       //动态计算偏移值


           xoffset = (qreal)values.length()*size/2.0;



           if(alingns[i]&Qt::AlignLeft)     //靠左
           {

               paint.drawText(QRect(TextPoint.x()+offest[i].x(),TextPoint.y()-size+offest[i].y(),xoffset*2,size*2),alingns[i],values);

           }

           else if(alingns[i]&Qt::AlignRight)     //靠右
           {

               paint.drawText(QRect(TextPoint.x()-xoffset*2+offest[i].x(),TextPoint.y()-size+offest[i].y(),xoffset*2,size*2),alingns[i],values);

           }
           else         //靠上
           {

               paint.drawText(QRect(TextPoint.x()-xoffset+offest[i].x(),TextPoint.y()+offest[i].y(),xoffset*2,size*2),alingns[i],values);

           }


           TextPoint = CustomRotate(TextPoint,TextRotate,40);   //获取点=210°的文字位置

           TextRotate-=40;
        }


        paint.restore();

}


void Dial::drawShade(QPainter &paint)              //绘制阴影
{

    paint.save();

    int PenWidth = radius*0.90-radius*0.77+4;


    paint.setPen(QPen(QBrush(QColor(255,255,255,30)),PenWidth));
    paint.setBrush(Qt::transparent);


    paint.drawArc(QRectF(-radius*0.90+PenWidth/2-3,-radius*0.90+PenWidth/2-3,radius*1.80-PenWidth+6,radius*1.80-PenWidth+6),-30*16+5*15,240*16-10*15);
    paint.restore();
}


void Dial::drawslideScaleColor(QPainter &paint)    //画划过的颜色
{
    if(modeColor == ModeSingleColor)
    {
        drawslideScaleSingleColor(paint);
    }
    else
    {
        drawslideScaleGradientColor(paint);
    }

}


void Dial::drawslideScaleSingleColor(QPainter &paint)      //绘制划过的单色
{

    int Star_Angle= 210*16-(int)(((value-minvalue)/(maxvalue-minvalue))*240*16);

    int spanAngle  =  210*16 - Star_Angle;


    if(spanAngle==0)
        return ;



    qreal SlideBottom =  ((qreal)radius*0.77)/((qreal)radius*0.90);    //滑动的底部比例

    qreal SlideCenterTop = 1-(1-SlideBottom)/3;                        //滑动的中心深色顶部比例
    qreal SlideCenterBottom = SlideBottom+(1-SlideBottom)/3+0.01;      //滑动的中心深色底部比例

    paint.save();

    QColor Tint_SlideColor = SingleSlideColor;


    QRadialGradient Radial(0,0,radius*0.90);


    Tint_SlideColor.setAlpha(40);
    Radial.setColorAt(1,Tint_SlideColor);
    Radial.setColorAt(SlideBottom-0.005,Tint_SlideColor);
    Radial.setColorAt(0,Qt::transparent);
    Radial.setColorAt(SlideBottom-0.006,Qt::transparent);


    Tint_SlideColor = SingleSlideColor;
    Tint_SlideColor.setAlpha(50);


    Radial.setColorAt(SlideCenterBottom-0.03,Tint_SlideColor);
    Radial.setColorAt(SlideCenterTop+0.03,Tint_SlideColor);

    Tint_SlideColor = SingleSlideColor;
    Tint_SlideColor.setAlpha(50);
    Radial.setColorAt(SlideCenterBottom-0.01,SingleSlideColor.darker(200));
    Radial.setColorAt(SlideCenterTop+0.01,SingleSlideColor.darker(200));

    Radial.setColorAt(SlideCenterBottom,SingleSlideColor);
    Radial.setColorAt(SlideCenterTop,SingleSlideColor);

    paint.setPen(Qt::NoPen);
    paint.setBrush(Radial);

    paint.drawPie(QRectF((qreal)-radius*0.90,(qreal)-radius*0.90,(qreal)radius*1.80,(qreal)radius*1.80),Star_Angle,spanAngle);

    paint.restore();

}




void Dial::drawslideScaleGradientColor(QPainter &paint)     //绘制划过的渐变色
{


    qreal angles[GradientSlideColor.count()];    //自定义渐变颜色位置

    qreal top=0.8666668;
    qreal bottom=0.8223336;


    int Star_Angle= 210*16-(int)(((value-minvalue)/(maxvalue-minvalue))*240*16);

    int spanAngle  =  210*16 - Star_Angle;



    if(spanAngle==0)
        return ;


    for(int i=0;i<GradientSlideColor.count();i++)
    {
        angles[GradientSlideColor.count()-i-1] =(qreal)(240/360.0)*(qreal) i/ (qreal)(GradientSlideColor.count()-1);

    }


    paint.save();

    QConicalGradient Conical(0,0,-30);

    /*绘制深渐变*/
    for(int i =0 ; i<GradientSlideColor.count(); i++)
         Conical.setColorAt(angles[i],GradientSlideColor[i]);

    Conical.setColorAt(angles[0]+0.01,Qt::transparent);
    Conical.setColorAt(1,Qt::transparent);

    paint.setPen(Qt::NoPen);
    paint.setBrush(Conical);

    paint.drawPie(QRectF((qreal)-radius*top,(qreal)-radius*top,(qreal)radius*top*2,(qreal)radius*top*2),Star_Angle,spanAngle);



    paint.setBrush(bkColor);
    paint.drawPie(QRectF((qreal)-radius*bottom,(qreal)-radius*bottom,(qreal)radius*bottom*2,(qreal)radius*bottom*2),Star_Angle,spanAngle);


    /*绘制浅渐变*/
    QList<QColor> Tint_Colors = GradientSlideColor;

    for(int i=0; i< GradientSlideColor.count();i++)
       Tint_Colors[i].setAlpha(90);

    for(int i =0 ; i<GradientSlideColor.count(); i++)
         Conical.setColorAt(angles[i],Tint_Colors[i]);


    paint.setBrush(Conical);
    paint.drawPie(QRectF((qreal)-radius*0.90,(qreal)-radius*0.90,(qreal)radius*1.80,(qreal)radius*1.80),Star_Angle,spanAngle);


    paint.restore();



}


void Dial::drawPointColor(QPainter &paint)          //绘制指针
{

    qreal PointTop;                //动态计算指针头
    qreal PointBottom;             //动态计算指针底部
    if(radius<=120)
    {
        PointTop = 3;
        PointBottom = 6;
    }
    else if((radius>120)&&(radius<500))
    {
        PointTop =3 + (radius-120)/100;
        PointBottom = PointTop*3;
    }
    else if(radius>=500)
    {
        PointTop = 3 + (radius-120)/140;
        PointBottom = PointTop*3;
    }

    //指针
    const QPointF  Pointer[4] = {
        QPointF(- PointTop / 2, radius*0.80),
        QPointF(PointTop / 2, radius*0.80),
        QPointF(PointBottom / 2, centerR*0.9),
        QPointF(-PointBottom / 2, centerR*0.9)
    };

    paint.save();


    paint.setBrush(PointerColor);
    paint.setPen(PointerColor.darker(150));


    qreal Current_Angle =60+(int)(((value-minvalue)/(maxvalue-minvalue))*240);

    paint.rotate(Current_Angle);

    paint.drawConvexPolygon(Pointer, 4);

    paint.restore();

}
void Dial::drawIconValueColor(QPainter &paint)           //绘制图标和value
{
    qreal IconSize;        // 图标大小
    qreal ValueSize;        // Value大小


    paint.save();

    if(!icon.isNull())
    {

        IconSize =  radius/5;

        icon.load(iconfile);        //从新装载,保证图片清晰度

        icon = icon.scaled(IconSize,IconSize,Qt::KeepAspectRatio);

        paint.drawPixmap(-IconSize/2,-centerR*0.80,IconSize,IconSize,icon);
    }




    QString text("%1");


    /*计算手动下的中心圆值*/
    QString text1= fetchDecimalPoint(text.arg(minvalue),center_decimal);
    QString text2= fetchDecimalPoint(text.arg(maxvalue),center_decimal);

    int size = qMax(text1.length(),text2.length());

    ValueSize = (qreal)(centerR*2)/(size+center_decimal+1);


    text= fetchDecimalPoint(text.arg(value),center_decimal);

    paint.setFont(QFont("DokChampa",ValueSize));
    paint.setPen(ValueColor);

    paint.drawText(QRectF(-centerR,-centerR*0.2,centerR*2,ValueSize*2),Qt::AlignCenter,Value_Text.arg(text)) ;

    paint.restore();
}

void Dial::drawlabelColor(QPainter &paint)           //绘制标签 比如绘制: km/h  kg/m3等等
{

    qreal ValueSize=centerR/4;

    paint.save();

    paint.setFont(QFont("Euphemia",ValueSize,QFont::Normal));
    paint.setPen(LabelColor);

    paint.drawText(QRectF(-ValueSize*6,centerR*1.2,ValueSize*12,ValueSize*2),Qt::AlignCenter,label) ;

    paint.restore();
}

// Angle_start Angle_end :表示圆盘的起始/结束角度,Angle_end必须大于Angle_start哦
//ratio:表示绘制的阴影亮度的扁与平(为0~1之间),为0表示平的圆环,为0.99表示最扁的圆环,大于等于1则不会绘制
//alpha:表示铺上的白色透明度
void Dial::drawDialShade(QPainter &paint,qreal Angle_start,qreal Angle_end,qreal ratio,int alpha)//给整个表盘绘制阴影
{
     #define PI   3.14159265

     int  size = 150;

     QColor White(255,255,255,alpha);

     qreal  Angle_Start = Angle_start*PI/180;            // 开始
     qreal  Angle_End = Angle_end*PI/180;                // 结束


     QPointF  point[size*2];           //阴影白色,上面为size个点,下面为size个点


     qreal  Round_Radius = radius*0.90; //阴影圆半径

     if((Angle_start>Angle_end)&&(ratio>0.9999))
            return ;

     paint.save();
     paint.setPen(Qt::NoPen);
     paint.setBrush(White);


     qreal top_Angle_spacing =  (Angle_End-Angle_Start)/size;    //间隔位置
     qreal top_current_Angle=0;

     //获取上面圆环的点位置
     for(int i =0;i<size;i++)
     {
         qreal currentX = Round_Radius*qCos(Angle_Start+top_current_Angle);
         qreal currentY = -Round_Radius*qSin(Angle_Start+top_current_Angle);

         point[i].setX(currentX);
         point[i].setY(currentY);
         top_current_Angle+=top_Angle_spacing;
     }


     //获取下面圆环的点位置
     if(ratio==0)       //平的圆环,不需要计算
     {
        paint.drawConvexPolygon(point,size);

     }
     else        //扁的圆环
     {
       qreal xStep = (point[0].x() - point[size-1].x())/size;
       qreal xCurrentStep=0;

       qreal yStep = (point[0].y() - point[size-1].y())/size;
       qreal yCurrentStep=0;

       for(int i =size;i<size*2;i++)
       {
          qreal currentBottomX= point[size-1].x()+xCurrentStep;         //获取当前底部圆环的点

          qreal currentBottomY=point[size-1].y()+yCurrentStep;

          qreal topX = point[size-i+size-1].x();                     //获取当前顶部圆环的点
          qreal topY = point[size-i+size-1].y();

//          /*更据比例,获取当前圆环的点*/
           qreal currentX =(topX-currentBottomX)*ratio+currentBottomX;
           qreal currentY =(topY-currentBottomY)*ratio+currentBottomY;


          point[i].setX(currentX);
          point[i].setY(currentY);

          xCurrentStep+=xStep;
          yCurrentStep+=yStep;
       }

        paint.drawConvexPolygon(point,size*2);
     }
     paint.restore();
}




void Dial::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.translate(width()/2,height()/2);

    radius = qMin(width(),height())/2;

    centerR=radius*0.38;       //设置中心圆大小


    drawObkColor(painter);           //外圆盘

    drawScalebkColor(painter);      //画刻度圆

    drawslideScaleColor(painter);   //画划过的颜色

    drawShade(painter);            //画阴影

    drawScaleColor(painter);

    drawbkColor(painter);          //画内圆

    drawScaleTextColor(painter);   //画刻度值


    drawPointColor(painter);
    drawCenterColor(painter);      //绘制中心圆
    drawIconValueColor(painter);
    drawlabelColor(painter);

    if(!isFlat)
    {
     drawDialShade(painter,70,150,0.40,5);         //绘制整个表盘阴影
     drawDialShade(painter,45,165,0.40,4);         //绘制整个表盘阴影
     drawDialShade(painter,30,180,0.33,40);        //绘制整个表盘阴影 30°~180°
    }


}



/*  point: 文字所在的点
 *  from_angle : 文字所在的度数
 *  rotate : 需要旋转的角度,值为-360~360(为负数表示逆时针旋转,为正数表示顺时针旋转)
 */

QPoint Dial::CustomRotate(QPointF point,qreal from_angle,qreal rotate)
{
    #define PI  3.14159265

    QPointF Tmp;
    qreal arc = (rotate-from_angle)/180*PI;
    qreal Length = qSqrt(point.x()*point.x() +point.y()*point.y());

    Tmp.setX(Length*qCos(arc));
    Tmp.setY(Length*qSin(arc));

    return Tmp.toPoint();
}

//通过给定的数,来获取要保留的数,比如:参数为12.234,由于decimal_point=1,所以返回值为12.3
QString Dial::fetchDecimalPoint(QString value,int point)    //value:值  point:要保留的小数位
{
    int  pos= value.indexOf(".");

    if(pos==-1)
       return value;

    pos +=point;

    if(point==0)        //去掉小数点
     pos -=1;

    if(pos>=value.length())
        return value;

    return value.left(pos+1);
}



void Dial::ontimeout()
{
    if(value!=endvalue)
    {

        if(timer_value==0)
        {
            value = endvalue;
            sendEnd();
        }
        else if( qAbs(value-endvalue)<=qAbs(timer_value))
        {
             value = endvalue;
             sendEnd();
        }
        else if(value>endvalue)
        {
             value -=qAbs(timer_value);
        }
        else
        {
             value +=qAbs(timer_value);
        }

        sendChangeValue();

        update();
    }

    if(value==endvalue)
    {
      timer.stop();
    }
}
void Dial::setFlat(bool isFlat)               //设置表盘是否是平面的
{
    this->isFlat =isFlat;
    update();
}

void Dial::setGradientColorMode(QList<QColor>& Qcolors)       //添加渐变
{

        GradientSlideColor.clear();
        GradientSlideColor=Qcolors;

        modeColor = ModeGradientColor;

        update();
}
void Dial::setSingleColorMode(QColor color)
{
     modeColor = ModeSingleColor;
     update();
}

bool Dial::set_Scale_decimal(int point)           //设置刻度值保留小数多少位
{
    if((point<0)&&(point>5))
        return false;

    Scale_decimal = point;
    update();
    return true;
}
bool Dial::set_Center_decimal(int point)           //设置中心数值保留小数多少位
{
    if((point<0)&&(point>5))
        return false;

    center_decimal = point;
    update();
    return true;
}

void Dial::setTimerType(int msec,int v)   //设置定时器参数,每过多少ms,跑多少值
{
    timer_value = v;
    timer_msec  = msec;
    if(timer_msec)
    {
        timer.setInterval(timer_msec);

    }
    else if(timer_msec==0)
    {
        timer.stop();
    }
}

qreal Dial::EndValue()                    //读取终点值
{
     return endvalue;
}
qreal Dial::CurrentValue()                //读取当前值
{
     return value;
}
qreal Dial::MinValue()                    //读取最小值
{
     return minvalue;
}
qreal Dial::MaxValue()                    //读取最大值
{
     return maxvalue;
}


bool Dial::setCurrentValue(qreal v)      //设置当前值(同时也会将终点值设为一样)
{
    if((v<minvalue)||(v>maxvalue))
    {
        return false;
    }

    value =  v;

    endvalue = v;

    update();

    return true;
}

bool Dial::setEndValue(qreal v)           //设置终点值
{

    if((v<minvalue)||(v>maxvalue))
    {
        return false;
    }

    endvalue = v;

    if(timer_msec==0)           //表示定时器不需要延时,直接跳到终点
    {

      value = v;

      sendEnd();
      sendChangeValue();
      update();
    }
    else if(value==v)
    {
        sendEnd();
        sendChangeValue();

    }
    else
    {
        if(!timer.isActive())
        {
           timer.start();
        }
    }

    return true;
}

bool Dial::setMinValue(qreal v)             //设置最小值
{
    if(v>value)
    {
        return false;
    }

    minvalue = v;

    if(minvalue>endvalue)
    {
        endvalue=minvalue;
    }

    update();

    return true;
}

bool Dial::setMaxValue(qreal v)             //设置最大值
{
    if(v<value)
    {
        return false;
    }
    maxvalue = v;

    if(maxvalue<endvalue)
    {
        endvalue=maxvalue;
    }

    update();

    return true;
}

//一次性设置所有值:当前值,最小值,最大值,终点值
bool Dial::setValues(qreal cV,qreal minV,qreal maxV,qreal endV)
{
    bool ret=true;

    if((cV>=minV)&&(cV<=maxV)&&(endV>=minV)&&(endV<=maxV))
    {
        endvalue =endV;

        value = cV;

        minvalue = minV;

        maxvalue = maxV;

        update();
    }
    else
    {
        ret = false;
    }

    return ret;
}



/*设置圆组件颜色*/
void Dial::setobkColor(QColor obkColor)
{
    this->obkColor=obkColor;
    update();
}
void Dial::setbkColor(QColor bkColor)
{
    this->bkColor=bkColor;
    update();
}

void Dial::setcentercolor(QColor centercolor)
{
    this->centercolor=centercolor;
    update();
}


/*设置指针组件颜色*/
void Dial::setPointerColor(QColor PointerColor)
{
    this->PointerColor=PointerColor;
    update();
}
/*设置文字颜色(刻度颜色,划过的刻度颜色,中心值颜色,标签值颜色)*/
void Dial::setScaleColor(QColor ScaleColor)
{
    this->ScaleColor=ScaleColor;
    update();
}

void Dial::setslideScaleColor(QColor slideScaleColor)
{
    this->slideScaleColor=slideScaleColor;
    update();
}

void Dial::setValueColor(QColor ValueColor)
{
    this->ValueColor=ValueColor;
    update();
}

void Dial::setLabelColor(QColor LabelColor)
{
    this->LabelColor=LabelColor;
    update();
}
