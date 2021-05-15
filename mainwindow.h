#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QPainter>
#include "plot.h"
#include "PlotBaseWidget.h"
#include "axistag.h"
#include "Dial.h"

// 接收缓冲区大小，单位字节
#define BufferSize      50


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


typedef enum
{
    three_phase,
    PID,
}plotSta;


//曲线属性
struct StLineInfo
{
    QString name;   //曲线名称
    QColor c;       //曲线颜色

    StLineInfo(){c = Qt::transparent;}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
//    explicit PlotBaseWidget(QWidget *parent = 0,int w = 400,int h = 300);

    /**
     * @brief 设置曲线信息
     * @param mapLineInfo 曲线信息，键值用于标识每条曲线
     */
    void CreateGraph(QMap<int,StLineInfo> mapLineInfo);

    /**
     * @brief 添加数据
     * @param key 键值
     * @param mapData 数据内容，键为数据类型标识，值为key对应的值
     */
    void AddData(double key,QMap<int,double> mapData);

    /**
     * @brief 设置游标标签的显示和隐藏
     * @param b 显示/隐藏
     */
    void ShowTagLabels(bool b);

    /**
     * @brief 设置数值游标的显示和隐藏
     * @param b 显示/隐藏
     */
    //void ShowValueTracer(bool b);

    /**
     * @brief 设置X轴范围
     * @param id 用于区别下侧X轴和上侧X轴，0-下侧，1上侧
     * @param lower 小值
     * @param upper 大值
     */
    void SetXrange(int id,double lower, double upper);

    /**
     * @brief 设置Y轴范围
     * @param id 用于区别左侧Y轴和右侧Y轴，0-左侧，1右侧
     * @param lower 小值
     private slots:
void on_txtPointCountY_2_cursorPositionChanged(int arg1, int arg2);

* @param upper 大值
     */
    void SetYrange(int id,double lower, double upper);

    /**
     * @brief 设置X轴窗口长度（动态曲线使用）
     * @param w 窗口长度
     */
    void SetXLength(int w);

    void slotShowValueTracer(QMouseEvent*);  //显示数值游标

private:
    void InitParam();                        //初始化参数
    QColor GetUsefullColor(int i);           //获取可用颜色

private:
    QCustomPlot* m_plot;                    //曲线窗口

    Dial*  VDial;        //电压表
    Dial*  ADial;        //电流表
    Dial*  SDial;        //速度表
    Dial*  NDial;        //扭矩表
    Dial*  TDial;        //温度表
    Dial*  PPDial;       //功率表

    QTimer *dataupdate;



    //定义曲线属性信息
    struct StLineInfoAll
    {
        QPointer<QCPGraph> graph;          //图层标识
        QPointer<PlotValueTracer> tag;     //游标
        QPointer<PlotValueTracer> vtrac;   //数值游标
        StLineInfo info;                   //曲线信息
        StLineInfoAll(){graph = 0; tag = 0;vtrac = 0;}
    };

    QMap<int,StLineInfoAll> m_mapLineInfo;  //曲线属性
    QList<QColor> m_listColorDef;           //默认颜色

    int m_nXlength;                         //X轴长度
    int m_nYchanged;                        //0=表示y变化，1表示y2变化







public slots:
    void mousePress(QMouseEvent *e);
    void mouseMove1(QMouseEvent *e);
    void mouseMove2(QMouseEvent *e);
    void bolDataupdate(void);
private:
    QCustomPlot *cmPlot;
    QCPItemTracer *tracer;
    QCPItemText *tracerLabel;


///////////////////////////////////////////////////////////////////////////////////

private slots:
    void on_btnStart_clicked();  //按钮按下
    void slotTimeout();          //定时器槽函数，用于模拟数据

private:

    QTimer* m_timer;            //定时器
    PlotBaseWidget* m_dock;     //曲线窗口

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 绘图事件
    void paintEvent(QPaintEvent *);

private slots:


    void on_btnSwitch_clicked();

    void serialPortRead_Slot();

    void on_btnSend_clicked();

    void on_btnClearRec_clicked();

    void on_btnClearSend_clicked();

    void on_chkRec_stateChanged(int arg1);

    void on_chkSend_stateChanged(int arg1);

    void on_chkShowLegend_stateChanged(int arg1);

    void on_chkTimSend_stateChanged(int arg1);




    void dataRateCalculate(void);

private:
    Ui::MainWindow *ui;

    // 波形绘图窗口
//    Plot *plot = NULL;// 必须初始化为空，否则后面NEW判断的时候会异常结束

    QSerialPort *mySerialPort;

    // 发送、接收字节计数
    long sendNum=0, recvNum=0, tSend=0, tRecv=0;// 发送/接收数量，历史发送/接收数量，Byte
    long sendRate=0, recvRate=0;// 发送/接收速率，Byte/s
    long recvFrameNum=0, recvFrameRate=0, recvErrorNum=0, tFrame=0;// 接收的有效帧数，帧速率，误码帧数量，历史帧数量
    QLabel *lblSendNum, *lblRecvNum, *lblSendRate, *lblRecvRate, *lblRecvFrameNum, *lblFrameRate;

    void setNumOnLabel(QLabel *lbl, QString strS, long num);

    /* 与帧过滤有关的标志位 */
    //int snum = 0;                               // 系统串口接收缓存区的可用字节数
    int tnum = 0;                               // 用户串口接收缓存的指针位置
    unsigned char chrtmp[BufferSize];           // 用户串口接收缓存，将缓存的数据放入这里处理
    unsigned char chrtmp1[BufferSize];           // 用户串口接收缓存，将缓存的数据放入这里处理


    void xFrameDataFilter(QByteArray *str, short value[]);
    //QByteArray xFrameDataFilter(QByteArray *str);

    // 定时发送-定时器
    QTimer *timSend;
    // 发送速率、接收速率统计-定时器
    QTimer *timRate;






public:
//    explicit Plot(QWidget *parent = nullptr);
//    ~Plot();
    // 绘图控件的指针
    QCustomPlot *pPlot1;
    int datavalid = 0;
    int datavalid1 = 0;

    void ShowPlot_TimeDemo(QCustomPlot *customPlot, double num);
    void ShowPlot_WaveForm(QCustomPlot *customPlot, float value[]);

private slots:
    void TimeData_Update(void);



    void on_txtPointCountX_returnPressed();

    void on_txtPointCountY_returnPressed();

    void on_btnColourBack_clicked();

    void on_txtPointOriginX_returnPressed();

    void on_chkTrackX_stateChanged(int arg1);

    void on_chkAdjustY_stateChanged(int arg1);

    void on_txtPointOriginY_returnPressed();

    void repPlotCoordinate();

    void on_btnClearGraphs_clicked();

    void on_txtMainScaleNumX_returnPressed();

    void on_txtMainScaleNumY_returnPressed();



public:
    // 定时器指针
    QTimer *timer;
    // 三相绘图控件中曲线的指针
    QCPGraph *pCurve[3];
    // 绘图框X轴显示的坐标点数
    int pointOriginX=0;
    int pointOriginY=0;
    int pointCountX=0;
    int pointCountY=0;

    int plotCount = 3;

    double cnt=0;// 当前绘图的X坐标



    // ui界面中，选择曲线可见性的checkBox的指针。方便用指针数组写代码，不然很占地方
    QCheckBox *pChkVisibleCurve[3];
    // ui界面中，选择曲线颜色的pushButton的指针。方便用指针数组写代码，不然很占地方
    QPushButton *pBtnColourCurve[3];
    // ui界面中，曲线当前值的lineEdit的指针。方便用指针数组写代码，不然很占地方
    QLineEdit *pTxtValueCurve[3];
    // ui界面中，选择曲线粗细的radioButton的指针。方便用指针数组写代码，不然很占地方
    QRadioButton *pRdoBoldCurve[3];
    // ui界面中，选择曲线样式的cmbLineStyle的指针。方便用指针数组写代码，不然很占地方
    QComboBox *pCmbLineStyle[3];
    // ui界面中，选择散点样式的cmbScatterStyle的指针。方便用指针数组写代码，不然很占地方
    QComboBox *pCmbScatterStyle[3];

    void QPlot_init(QCustomPlot *customPlot);
    void QPlot_widget_init(void);

    void curveSetVisible(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetColor(QCustomPlot *pPlot, QCPGraph *pCurve, QPushButton *btn);
    void curveSetBold(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetLineStyle(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetScatterStyle(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);

    void setAutoTrackX(QCustomPlot *pPlot);
    void setManualSettingX(QCustomPlot *pPlot);
    void setAutoTrackY(QCustomPlot *pPlot);





    // 绘图控件的指针
    QCustomPlot *pPlot2;

    void ShowPlot_TimeDemo1(QCustomPlot *customPlot, double num);
    void ShowPlot_WaveForm1(QCustomPlot *customPlot, float value[]);

private slots:
    void TimeData_Update1(void);



    void on_txtPointCountX_2_returnPressed();

    void on_txtPointCountY_2_returnPressed();

    void on_btnColourBack_2_clicked();

    void on_txtPointOriginX_2_returnPressed();

    void on_chkTrackX_2_stateChanged(int arg1);

    void on_chkAdjustY_2_stateChanged(int arg1);

    void on_txtPointOriginY_2_returnPressed();

    void repPlotCoordinate_2();

    void on_btnClearGraphs_2_clicked();

    void on_txtMainScaleNumX_2_returnPressed();

    void on_txtMainScaleNumY_2_returnPressed();








    //void on_chkDrawDemo_2_stateChanged(int arg1);



    void on_chkShowLegend_2_stateChanged(int arg1);











    void on_txtMainScaleNumY_3_returnPressed();

public:
    // 定时器指针
    QTimer *timer1;
    // 三相绘图控件中曲线的指针
    QCPGraph *pCurve1[3];
    // 绘图框X轴显示的坐标点数
    int pointOriginX1=0;
    int pointOriginY1=0;
    int pointCountX1=0;
    int pointCountY1=0;

    int plotCount1 = 3;

    double cnt1=0;// 当前绘图的X坐标

    // ui界面中，选择曲线可见性的checkBox的指针。方便用指针数组写代码，不然很占地方
    QCheckBox *pChkVisibleCurve1[3];
    // ui界面中，选择曲线颜色的pushButton的指针。方便用指针数组写代码，不然很占地方
    QPushButton *pBtnColourCurve1[3];
    // ui界面中，曲线当前值的lineEdit的指针。方便用指针数组写代码，不然很占地方
    QLineEdit *pTxtValueCurve1[3];
    // ui界面中，选择曲线粗细的radioButton的指针。方便用指针数组写代码，不然很占地方
    QRadioButton *pRdoBoldCurve1[3];
    // ui界面中，选择曲线样式的cmbLineStyle的指针。方便用指针数组写代码，不然很占地方
    QComboBox *pCmbLineStyle1[3];
    // ui界面中，选择散点样式的cmbScatterStyle的指针。方便用指针数组写代码，不然很占地方
    QComboBox *pCmbScatterStyle1[3];

    void QPlot_init1(QCustomPlot *customPlot);
    void QPlot_widget_init1(void);

    void curveSetVisible_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetColor_2(QCustomPlot *pPlot, QCPGraph *pCurve, QPushButton *btn);
    void curveSetBold_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetLineStyle_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);
    void curveSetScatterStyle_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1);

    void setAutoTrackX_2(QCustomPlot *pPlot);
    void setManualSettingX_2(QCustomPlot *pPlot);
    void setAutoTrackY_2(QCustomPlot *pPlot);
};
#endif // MAINWINDOW_H
