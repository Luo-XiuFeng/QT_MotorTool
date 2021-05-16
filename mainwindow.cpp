#include "mainwindow.h"
#include "Dial.h"
#include "PlotBaseWidget.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <stdio.h>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MotorCtrl");
    setWindowIcon(QIcon(":M.ico"));

    // 状态栏
    QStatusBar *sBar = statusBar();
    // 状态栏的收、发计数标签
    lblSendNum = new QLabel(this);
    lblRecvNum = new QLabel(this);
    lblSendRate = new QLabel(this);
    lblRecvRate = new QLabel(this);
    // 设置标签最小大小
    lblSendNum->setMinimumSize(100, 20);
    lblRecvNum->setMinimumSize(100, 20);
    lblSendRate->setMinimumSize(100, 20);
    lblRecvRate->setMinimumSize(100, 20);
    //statusBar()->showMessage("留言", 5000);// 留言显示，过期时间单位为ms，过期后不再有显示
    //statusBar()->setSizeGripEnabled(false); // 是否显示右下角拖放控制点，默认显示
    //statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 设置不显示label的边框
    //lblSendNum->setAlignment(Qt::AlignHCenter);// 设置label属性
    //sBar->addPermanentWidget();//addSeparator();// 添加分割线，不能用
    // 状态栏显示计数值
    //lblSendNum->setText("S: 0");
    //lblRecvNum->setText("R: 0");
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
    setNumOnLabel(lblSendRate, "Byte/s: ", 0);
    setNumOnLabel(lblRecvRate, "Byte/s: ", 0);
    // 从右往左依次添加
    sBar->addPermanentWidget(lblSendNum);
    sBar->addPermanentWidget(lblSendRate);
    sBar->addPermanentWidget(lblRecvNum);
    sBar->addPermanentWidget(lblRecvRate);

    // 状态栏添加超链接
    QLabel *lblLinkBlog = NULL;
    QLabel *lblLinkSource =NULL;
    lblLinkBlog = new QLabel(this);
    lblLinkBlog->setOpenExternalLinks(true);
    //lblLinkBlog->setText("<a href=\"https://blog.csdn.net/Mark_md/article/details/108928314\">");// 有下划线
    lblLinkBlog->setText("<style> a {text-decoration: none} </style> <a href=\"https://blog.csdn.net/Mark_md/article/details/109995490\">");// 无下划线
    lblLinkSource = new QLabel(this);
    lblLinkSource->setOpenExternalLinks(true);
    //lblLinkSource->setText("<a href=\"https://github.com/ZhiliangMa/Qt-SerialDebuger\">");
    lblLinkSource->setText("<style> a {text-decoration: none} </style> <a href=\"https://github.com/ZhiliangMa/Qt_SerialPlot\">");// 无下划线
    lblLinkBlog->setMinimumSize(40, 20);
    lblLinkSource->setMinimumSize(60, 20);
    // 从左往右依次添加
    sBar->addWidget(lblLinkBlog);
    sBar->addWidget(lblLinkSource);

    lblRecvFrameNum = new QLabel(this);
    lblFrameRate = new QLabel(this);
    lblRecvFrameNum->setMinimumSize(100, 20);
    lblFrameRate->setMinimumSize(80, 20);
    setNumOnLabel(lblRecvFrameNum, "FNum: ", recvFrameNum);
    setNumOnLabel(lblFrameRate, "FPS: ", recvFrameRate);
    // 从右往左依次添加
    sBar->addPermanentWidget(lblRecvFrameNum);
    sBar->addPermanentWidget(lblFrameRate);

    // 定时发送-定时器
//    timSend = new QTimer;
//    timSend->setInterval(1000);// 设置默认定时时长1000ms
//    connect(timSend, &QTimer::timeout, this, [=](){
//        on_btnSend_clicked();
//    });

    // 发送速率、接收速率统计-定时器
    timRate = new QTimer;
    timRate->start(1000);
    connect(timRate, &QTimer::timeout, this, [=](){
        dataRateCalculate();
    });

    // 新建一串口对象
    mySerialPort = new QSerialPort(this);

    // 串口接收，信号槽关联
    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(serialPortRead_Slot()));

    // 隐藏高级收码显示区域
//    ui->widget_5->hide();
//    ui->frame_3->hide();

    // 新建波形显示界面
    pPlot1 = ui->winPlot;
    QPlot_init(pPlot1);

    // 绘图图表的设置控件初始化，主要用于关联控件的信号槽
    QPlot_widget_init();



    // 创建定时器1
    timer = new QTimer(this);
    timer->setInterval(1);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimeData_Update()));
    timer->start(10);

    // 关联控件初始化
    ui->txtPointOriginX->setEnabled(false);
    // 图表重绘后，刷新原点坐标和范围
    connect(pPlot1,SIGNAL(afterReplot()),this,SLOT(repPlotCoordinate()));


//    tracer = new QCPItemTracer(pPlot1); //生成游标
//    //下面的代码就是设置游标的外观
//    tracer->setPen(QPen(Qt::red));
//    tracer->setBrush(QBrush(Qt::red));
//    tracer->setStyle(QCPItemTracer::tsSquare);
//    tracer->setSize(5.0);

//    tracerLabel = new QCPItemText(pPlot1); //生成游标说明
//    //下面的代码就是设置游标说明的外观和对齐方式等状态
//    tracerLabel->setLayer("overlay");
//    tracerLabel->setPen(QPen(Qt::NoPen));
//    tracerLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);//文字布局：左对齐、顶
//    tracerLabel->setFont(QFont(font().family(), 15)); //字体大小
//    //下面这个语句很重要，它将游标说明锚固在tracer位置处，实现自动跟随
//    tracerLabel->position->setParentAnchor(tracer->position);

//    //这里的信号-槽连接语句很重要
//    connect(pPlot1, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove1(QMouseEvent*)));
//    connect(pPlot1, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove2(QMouseEvent*)));


    pPlot2 = ui->winPlot_2;
    QPlot_init1(pPlot2);

    // 绘图图表的设置控件初始化，主要用于关联控件的信号槽
    QPlot_widget_init1();

    timer1 = new QTimer(this);
    timer1->setInterval(1);
    connect(timer1,SIGNAL(timeout()),this,SLOT(TimeData_Update1()));
    timer1->start(10);

    // 关联控件初始化
    ui->txtPointOriginX_2->setEnabled(false);
    // 图表重绘后，刷新原点坐标和范围
    connect(pPlot2,SIGNAL(afterReplot()),this,SLOT(repPlotCoordinate_2()));



//    connect(pPlot1,SIGNAL(mousePress(QMouseEvent *)), this, SLOT(mousePress(QMouseEvent*)));

    VDial = ui->widget;
    VDial->label="母线电压";
    VDial->Value_Text = "%1V";
    VDial->iconfile = ":img/V";
    VDial->icon = QPixmap(":img/V");
    VDial->setMinimumSize(270,240);
    VDial->setValues(0,0,48,0);
    VDial->setTimerType(40,4);
    VDial->Scale_decimal = false;
//    VDial->isFlat = true;
//    VDial->SingleSlideColor =QColor(0,0,0);           //划过的单颜色
//    VDial->slideScaleColor = QColor(150,150,150);     //划过的刻度颜色
    VDial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));


    ADial = ui->widget_2;
    ADial->label="电流";
    ADial->Value_Text = "%1A";
    ADial->iconfile = ":img/A";
    ADial->icon = QPixmap(":img/A");
    ADial->setMinimumSize(270,240);
    ADial->setValues(0,-50,50,0);
    ADial->setTimerType(40,4);
    ADial->Scale_decimal = false;
//    ADial->isFlat = true;
    ADial->SingleSlideColor =QColor(0,0,0);           //划过的单颜色
    ADial->slideScaleColor = QColor(150,150,150);     //划过的刻度颜色

//    ADial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));

    SDial = ui->widget_4;
    SDial->label="转速";
    SDial->Value_Text = "%1RPM";
    SDial->iconfile = ":img/S";
    SDial->icon = QPixmap(":img/S");
    SDial->setMinimumSize(270,240);
    SDial->setValues(0,-3000,3000,0);
    SDial->setTimerType(40,4);
    SDial->Scale_decimal = false;
    SDial->SingleSlideColor =QColor(0,0,0);           //划过的单颜色
    SDial->slideScaleColor = QColor(150,150,150);     //划过的刻度颜色

//    SDial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));


    NDial = ui->widget_10;
    NDial->label="扭矩";
    NDial->Value_Text = "%1Nm";
    NDial->iconfile = ":img/P";
    NDial->icon = QPixmap(":img/P");
    NDial->setMinimumSize(270,240);
    NDial->setValues(0,0,100,0);
    NDial->setTimerType(40,4);
    NDial->Scale_decimal = false;
    NDial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));


    PPDial = ui->widget_11;
    PPDial->label="功率";
    PPDial->Value_Text = "%1W";
    PPDial->iconfile = ":img/PP";
    PPDial->icon = QPixmap(":img/PP");
    PPDial->setMinimumSize(270,240);
    PPDial->setValues(0,0,1000,0);
    PPDial->setTimerType(40,4);
    PPDial->Scale_decimal = false;
    PPDial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));



    TDial = ui->widget_9;
    TDial->label="温度";
    TDial->Value_Text = "%1°";
    TDial->iconfile = ":img/T";
    TDial->icon = QPixmap(":img/T");
    TDial->setMinimumSize(270,240);
    TDial->setValues(0,0,120,0);
    TDial->setTimerType(40,4);
    TDial->Scale_decimal = false;
    TDial->setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));


    dataupdate = new QTimer(this);
    dataupdate->setInterval(1);
    connect(dataupdate,SIGNAL(timeout()),this,SLOT(bolDataupdate()));
    dataupdate->start(100);

}

void MainWindow::bolDataupdate(void)
{
//    VDial->setEndValue((float)chrtmp[20]);
//    ADial->setEndValue((float)chrtmp[12]);
}





void MainWindow::mousePress(QMouseEvent *e)
{
    static char presssta = 0;

    if(!presssta)
    {
        ui->chkTrackX->setCheckState(Qt::Unchecked);
        ui->txtPointOriginX->setEnabled(false);
        setAutoTrackX(pPlot1);
        pPlot1->replot(QCustomPlot::rpQueuedReplot);
        presssta = true;
    }
    else
    {
        ui->chkTrackX->setCheckState(Qt::Checked);
        ui->txtPointOriginX->setEnabled(true);
        presssta =false;
    }
}



void MainWindow::mouseMove1(QMouseEvent *e)
{
    //获得鼠标位置处对应的横坐标数据x
    double x = pPlot1->xAxis->pixelToCoord(e->pos().x());
    double xValue, yValue;
    //xValue就是游标的横坐标
    xValue = x;
    //yValue就是游标的纵坐标，这里直接根据产生数据的函数（sin）获得
    yValue = sin(xValue);
    //下面设置游标（tracer）的位置
    tracer->position->setCoords(xValue, yValue);
    //设置游标说明（tracerLabel）的内容
    tracerLabel->setText(QString("x = %1, y = %2").arg(xValue).arg(yValue));
    pPlot1->replot();//绘制器一定要重绘，否则看不到游标位置更新情况
}

void MainWindow::mouseMove2(QMouseEvent *e)
{
    //获得鼠标位置处对应的横坐标数据x
    double x = pPlot1->xAxis->pixelToCoord(e->pos().x());
    //下面的代码很关键
    tracer->setGraph(pPlot1->graph(0)); //将游标和该曲线图层想连接
    tracer->setGraphKey(x); //将游标横坐标（key）设置成刚获得的横坐标数据x
    tracer->setInterpolating(true); //游标的纵坐标可以通过曲线数据线性插值自动获得（这就不用手动去计算了）
    tracer->updatePosition(); //使得刚设置游标的横纵坐标位置生效
    //以下代码用于更新游标说明的内容

    double xValue = tracer->position->key();
    double yValue = tracer->position->value();

    tracerLabel->setText(QString("y = %2").arg(yValue));
//    tracerLabel->setText(QString("x = %1, y = %2").arg(xValue).arg(yValue));
    pPlot1->replot(); //不要忘了重绘
}


//开始-终止模拟
void MainWindow::on_btnStart_clicked()
{

}

// 模拟数据
void MainWindow::slotTimeout()
{

}


MainWindow::~MainWindow()
{
//    delete plot;
    delete ui;
}

// 绘图事件
void MainWindow::paintEvent(QPaintEvent *)
{
    // 绘图
    // 实例化画家对象，this指定绘图设备
    QPainter painter(this);

    // 设置画笔颜色
    QPen pen(QColor(0,0,0));
    // 设置画笔线宽（只对点线圆起作用，对文字不起作用）
    pen.setWidth(1);
    // 设置画笔线条风格，默认是SolidLine实线
    // DashLine虚线，DotLine点线，DashDotLine、DashDotDotLine点划线
    pen.setStyle(Qt::DashDotDotLine);
    // 让画家使用这个画笔
    painter.setPen(pen);

    //painter.drawLine(QPoint(ui->txtRec->x() + ui->txtRec->width(), ui->txtRec->y()), QPoint(this->width(), ui->txtRec->y()));
    //painter.drawLine(QPoint(ui->txtSend->x() + ui->txtSend->width(), ui->txtSend->y()), QPoint(this->width(), ui->txtSend->y()));
    painter.drawLine(QPoint(ui->statusbar->x(), ui->statusbar->y()-2), QPoint(this->width(), ui->statusbar->y()-2));

}

// 串口接收显示，槽函数
void MainWindow::serialPortRead_Slot()
{
    /*QString recBuf;
    recBuf = QString(mySerialPort->readAll());*/

    QByteArray recBuf;
    recBuf = mySerialPort->readAll();

    /* 帧过滤部分代码 */
    short wmValue[20] = {0};
    xFrameDataFilter(&recBuf, wmValue);

    // 调试信息输出，显示缓冲区内容（16进制显示）及接收标志位
//    if(!ui->widget_5->isHidden()){
//        QByteArray str1;
//        //for(int i=0; i<(tnum + 1); i++)
//        for(int i=0; i<BufferSize; i++)
//        {
//            str1.append(chrtmp[i]);
//        }
//        //ui->txtFrameTemp->setPlainText(str1.toHex().toUpper());
//        str1 = str1.toHex().toUpper();
//        QString str2;
//        for(int i = 0; i<str1.length (); i+=2)
//        {
//            str2 += str1.mid (i,2);
//            str2 += " ";
//        }
//        ui->txtFrameBuffer->setPlainText(str2);
        // 显示标志位
//        ui->txtFrameTnum->setText(QString::number(tnum));
//        ui->txtFrameH1->setText(QString::number(f_h1_flag));
//        ui->txtFrameH->setText(QString::number(f_h_flag));
//        ui->txtFrameFun->setText(QString::number(f_fun_word));
//        ui->txtFrameLen->setText(QString::number(f_length));
//        ui->txtFrameErrorNum->setText(QString::number(recvErrorNum));
//    }

    // 接收字节计数
    recvNum += recBuf.size();
    // 状态栏显示计数值
    setNumOnLabel(lblRecvNum, "R: ", recvNum);

    // 判断是否为16进制接收，将以后接收的数据全部转换为16进制显示（先前接收的部分在多选框槽函数中进行转换。最好多选框和接收区组成一个自定义控件，方便以后调用）
    if(ui->chkRec->checkState() == false){
        // GB2312编码输入
        QString strb = QString::fromLocal8Bit(recBuf);//QString::fromUtf8(recBuf);//QString::fromLatin1(recBuf);
        // 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
//        ui->txtRec->insertPlainText(strb);
    }else{
        // 16进制显示，并转换为大写
        QString str1 = recBuf.toHex().toUpper();//.data();
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
//        ui->txtRec->insertPlainText(str2);
        //ui->txtRec->insertPlainText(recBuf.toHex());
    }

    // 移动光标到文本结尾
//    ui->txtRec->moveCursor(QTextCursor::End);

    // 将文本追加到末尾显示，会导致插入的文本换行
    /*ui->txtRec->appendPlainText(recBuf);*/

    /*// 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
    ui->txtRec->insertPlainText(recBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去获取消息框文本，再将新接收到的消息添加到QString尾部，但感觉效率会比当前位置插入低。也不会发生换行
    /*QString txtBuf;
    txtBuf = ui->txtRec->toPlainText();
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去缓存接收到的所有消息，效率会比上面高一点。但清空接收的时候，要将QString一并清空。
    /*static QString txtBuf;
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/
}

// 打开/关闭串口 槽函数
void MainWindow::on_btnSwitch_clicked()
{

    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity   checkBits;

    mySerialPort->clear();

    // 获取串口波特率
    baudRate = (QSerialPort::BaudRate)ui->cmbBaudRate->currentText().toUInt();
    // 获取串口数据位
    dataBits = (QSerialPort::DataBits)ui->cmbData->currentText().toUInt();
    // 获取串口停止位
    if(ui->cmbStop->currentText() == "1"){
        stopBits = QSerialPort::OneStop;
    }else if(ui->cmbStop->currentText() == "1.5"){
        stopBits = QSerialPort::OneAndHalfStop;
    }else if(ui->cmbStop->currentText() == "2"){
        stopBits = QSerialPort::TwoStop;
    }else{
        stopBits = QSerialPort::OneStop;
    }

    // 获取串口奇偶校验位
    if(ui->cmbCheck->currentText() == "无"){
        checkBits = QSerialPort::NoParity;
    }else if(ui->cmbCheck->currentText() == "奇校验"){
        checkBits = QSerialPort::OddParity;
    }else if(ui->cmbCheck->currentText() == "偶校验"){
        checkBits = QSerialPort::EvenParity;
    }else{
        checkBits = QSerialPort::NoParity;
    }

    // 想想用 substr strchr怎么从带有信息的字符串中提前串口号字符串
    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    mySerialPort->setBaudRate(baudRate);
    mySerialPort->setDataBits(dataBits);
    mySerialPort->setStopBits(stopBits);
    mySerialPort->setParity(checkBits);
    //mySerialPort->setPortName(ui->cmbSerialPort->currentText());// 不匹配带有串口设备信息的文本
    // 匹配带有串口设备信息的文本
    QString spTxt = ui->cmbSerialPort->currentText();
    spTxt = spTxt.section(':', 0, 0);//spTxt.mid(0, spTxt.indexOf(":"));
    //qDebug() << spTxt;
    mySerialPort->setPortName(spTxt);

    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui->btnSwitch->text() == "打开串口"){
        if(mySerialPort->open(QIODevice::ReadWrite) == true){
            //QMessageBox::
            ui->btnSwitch->setText("关闭串口");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui->cmbSerialPort->setEnabled(false);
            ui->cmbBaudRate->setEnabled(false);
            ui->cmbStop->setEnabled(false);
            ui->cmbData->setEnabled(false);
            ui->cmbCheck->setEnabled(false);
        }else{
            QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n\r\n该串口可能被占用，请选择正确的串口\r\n或者波特率过高，超出硬件支持范围");
        }
    }else{
        mySerialPort->close();
        ui->btnSwitch->setText("打开串口");
        // 端口号下拉框恢复可选，避免误操作
        ui->cmbSerialPort->setEnabled(true);
        ui->cmbBaudRate->setEnabled(true);
        ui->cmbStop->setEnabled(true);
        ui->cmbData->setEnabled(true);
        ui->cmbCheck->setEnabled(true);
    }

}

// 发送按键槽函数
// 如果勾选16进制发送，按照asc2的16进制发送
void MainWindow::on_btnSend_clicked()
{
    QByteArray sendData;
    // 判断是否为16进制发送，将发送区全部的asc2转换为16进制字符串显示，发送的时候转换为16进制发送
    if(ui->chkSend->checkState() == false){
        // 字符串形式发送，GB2312编码用以兼容大多数单片机
        sendData = ui->txtSend->toPlainText().toLocal8Bit();// GB2312编码输出
        //sendData = ui->txtSend->toPlainText().toUtf8();// Unicode编码输出
        //sendData = ui->txtSend->toPlainText().toLatin1();
    }else{
        // 16进制发送，不要用.data()，.data()返回的是字符数组，0x00在ASC2中的意义为NUL，也就是'\0'结束符，所以遇到0x00就会终止
        //sendData = QByteArray::fromHex(ui->txtSend->toPlainText().toUtf8());// Unicode编码输出
        sendData = QByteArray::fromHex(ui->txtSend->toPlainText().toLocal8Bit());// GB2312编码输出
    }

    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int a = mySerialPort->write(sendData);
    // 发送字节计数并显示
    if(a > 0)
    {
        // 发送字节计数
        sendNum += a;
        // 状态栏显示计数值
        setNumOnLabel(lblSendNum, "S: ", sendNum);
    }

}

// 状态栏标签显示计数值
void MainWindow::setNumOnLabel(QLabel *lbl, QString strS, long num)
{
    // 标签显示
    QString strN;
    strN.sprintf("%ld", num);
    QString str = strS + strN;
    lbl->setText(str);
}

void MainWindow::on_btnClearRec_clicked()
{
//    ui->txtRec->clear();
    // 清除发送、接收字节计数
    sendNum = 0;
    recvNum = 0;
    tSend = 0;
    tRecv = 0;
    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
    // 清空帧数量
    recvFrameNum=0, recvFrameRate=0, recvErrorNum=0, tFrame=0;
    setNumOnLabel(lblRecvFrameNum, "FNum: ", recvFrameNum);
//    ui->txtFrameErrorNum->setText(QString::number(recvErrorNum));
}

void MainWindow::on_btnClearSend_clicked()
{
    ui->txtSend->clear();
    // 清除发送字节计数
    sendNum = 0;
    tSend = 0;
    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
}

// 先前接收的部分在多选框状态转换槽函数中进行转换。（最好多选框和接收区组成一个自定义控件，方便以后调用）
void MainWindow::on_chkRec_stateChanged(int arg1)
{
    // 获取文本字符串
//    QString txtBuf = ui->txtRec->toPlainText();

    // 获取多选框状态，未选为0，选中为2
    // 为0时，多选框未被勾选，接收区先前接收的16进制数据转换为asc2字符串格式
    if(arg1 == 0){

        //QString str1 = QByteArray::fromHex(txtBuf.toUtf8());
        //QString str1 = QByteArray::fromHex(txtBuf.toLocal8Bit());
        //把gb2312编码转换成unicode
//        QString str1 = QTextCodec::codecForName("GB2312")->toUnicode(QByteArray::fromHex(txtBuf.toLocal8Bit()));
        // 文本控件清屏，显示新文本
//        ui->txtRec->clear();
//        ui->txtRec->insertPlainText(str1);
        // 移动光标到文本结尾
//        ui->txtRec->moveCursor(QTextCursor::End);

    }else{// 不为0时，多选框被勾选，接收区先前接收asc2字符串转换为16进制显示

        //QString str1 = txtBuf.toUtf8().toHex().toUpper();// Unicode编码输出
//        QString str1 = txtBuf.toLocal8Bit().toHex().toUpper();// GB2312编码输出
        // 添加空格
        QByteArray str2;
//        for(int i = 0; i<str1.length (); i+=2)
//        {
//            str2 += str1.mid (i,2);
//            str2 += " ";
//        }
        // 文本控件清屏，显示新文本
//        ui->txtRec->clear();
//        ui->txtRec->insertPlainText(str2);
        // 移动光标到文本结尾
//        ui->txtRec->moveCursor(QTextCursor::End);

    }
}

// 先前发送区的部分在多选框状态转换槽函数中进行转换。（最好多选框和发送区组成一个自定义控件，方便以后调用）
void MainWindow::on_chkSend_stateChanged(int arg1)
{
    // 获取文本字符串
    QString txtBuf = ui->txtSend->toPlainText();

    // 获取多选框状态，未选为0，选中为2
    // 为0时，多选框未被勾选，将先前的发送区的16进制字符串转换为asc2字符串
    if(arg1 == 0){

        //QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());//仅能处理Unicode编码，因为QString就是Unicode
        //QString str1 = QString::fromLocal8Bit(txtBuf.toUtf8());//仅能处理GB2312编码，Unicode的数据无论如何都会乱码
        //把gb2312编码转换成unicode
        QString str1 = QTextCodec::codecForName("GB2312")->toUnicode(QByteArray::fromHex(txtBuf.toLocal8Bit()));
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->txtSend->moveCursor(QTextCursor::End);

    }else{// 多选框被勾选，将先前的发送区的asc2字符串转换为16进制字符串

        //QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();// Unicode编码输出
        QString str1 = txtBuf.toLocal8Bit().toHex().toUpper();// GB2312编码输出
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->txtSend->moveCursor(QTextCursor::End);

    }
}

// 定时发送开关 选择复选框
void MainWindow::on_chkTimSend_stateChanged(int arg1)
{
    // 获取复选框状态，未选为0，选中为2
    if(arg1 == 0){
        timSend->stop();
        // 时间输入框恢复可选
        ui->txtSendMs->setEnabled(true);
    }else{
        // 对输入的值做限幅，小于20ms会弹出对话框提示
        if(ui->txtSendMs->text().toInt() >= 20){
            timSend->start(ui->txtSendMs->text().toInt());// 设置定时时长，重新计数
            // 让时间输入框不可选，避免误操作（输入功能不可用，控件背景为灰色）
            ui->txtSendMs->setEnabled(false);
        }else{
            ui->chkTimSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 20ms\r\n请确保输入的值 >=20");
        }
    }
}

// 发送速率、接收速率统计-定时器
void MainWindow::dataRateCalculate(void)
{
    sendRate = sendNum - tSend; // * ui->cmbData->currentText().toUInt();
    recvRate = recvNum - tRecv; // * ui->cmbData->currentText().toUInt();
    recvFrameRate = recvFrameNum - tFrame;

    setNumOnLabel(lblSendRate, "Byte/s: ", sendRate);
    setNumOnLabel(lblRecvRate, "Byte/s: ", recvRate);
    setNumOnLabel(lblFrameRate, "FPS: ", recvFrameRate);
    tSend = sendNum;
    tRecv = recvNum;
    tFrame = recvFrameNum;
}



/*
void MainWindow::xFrameDataFilter(QByteArray *str)
{
    int num = str->size();
    if(num)
    {

    }
}*/

// 帧过滤
// 适用于有帧头、功能字、有效字段长度、校验位的接收，无帧尾
void MainWindow::xFrameDataFilter(QByteArray *str, short value[])
{
    int num = str->size();
    value[0] = value[0];


    if(num)
    {
        for(int i=0; i<num; i++)
        {
            chrtmp[i] = str->at(i);
        }
        datavalid  = true;
        datavalid1 = true;
    }


//    float val = 0;

//    memcpy(&val,chrtmp,sizeof (val));



//    ShowPlot_WaveForm(pPlot1, (float*)&chrtmp);
//    ShowPlot_WaveForm1(pPlot2, (float*)&chrtmp1);





//    if(num)
//    {
//        for(int i=0; i<num; i++)
//        {
//            chrtmp[tnum] = str->at(i);  		// 从接收缓存区读取一个字节
//            if (f_h_flag == 1)  // 有帧头。判断功能字、有效字段长度，接收消息
//            {
//                if (f_fun_word) // 有帧头，有功能字
//                {
//                    if (f_length) // 有帧头，有功能字，有有效字节长度
//                    {
//                        if((tnum-4) <f_length) // 有帧头，有功能字，未超出有效字节长度+校验位，接收数据
//                        {
//                            tnum ++;
//                        }
//                        else // 有帧头，有功能字，超出有效字节长度。判断校验位
//                        {
//                            // 累加和校验计算
//                            unsigned char crc=0;
//                            for(i=0; i<tnum;i++)
//                            {
//                                crc += chrtmp[i];
//                            }

//                            // 校验对比
//                            if(crc == chrtmp[tnum]) // 校验通过，将缓冲区的数据打包发送
//                            {
//                                ++recvFrameNum;// 有效帧数量计数
//                                setNumOnLabel(lblRecvFrameNum, "FNum: ", recvFrameNum);

//                                // 调试信息输出，显示有效帧的内容（16进制显示）
////                                if(!ui->widget_5->isHidden()){
////                                    QByteArray str1;
////                                    for(int i=0; i<(tnum+1); i++)
////                                    {
////                                        str1.append(chrtmp[i]);
////                                    }
////                                    //ui->txtFrameEffective->appendPlainText(str1.toHex().toUpper());
////                                    str1 = str1.toHex().toUpper();
////                                    QString str2;
////                                    for(int i = 0; i<str1.length (); i+=2)
////                                    {
////                                        str2 += str1.mid (i,2);
////                                        str2 += " ";
////                                    }
//////                                    ui->txtFrameEffective->appendPlainText(str2);
////                                }

//                                // 根据功能字进行功能解析，自动根据帧长度解析为对应的short值。
//                                if(f_fun_word == FunWord_WF)
//                                {
//                                    for(int i=0; i<(f_length/2); i++)
//                                    {
//                                        value[i] = ((short)chrtmp[i*2+4] << 8) | chrtmp[i*2+4+1];
//                                    }
//                                }

//                                // 显示波形（在这里显示可以处理多帧粘包，避免多帧粘包只显示一帧的情况）
//                                // 将解析出的short数组，传入波形图，进行绘图
////                                if(!plot->isHidden()){
////                                    plot->ShowPlot_WaveForm(plot->pPlot1, value);
////                                }

//                            }else{
//                                ++recvErrorNum;// 误码帧数量计数
//                            }

//                            // 清0重新接收
//                            tnum = 0;
//                            // 清空标志位
//                            f_h1_flag = 0;
//                            f_h_flag = 0;
//                            f_fun_word = 0;
//                            f_length = 0;

//                        }
//                        // 把上面下面的判断标志位 == 1去掉


//                    }           // 有帧头，有功能字，判断是否是有效字节长度
//                    else
//                    {
//                        if(chrtmp[tnum] <= ValidByteLength)
//                        {
//                            f_length = chrtmp[tnum];// 记录当前帧的有效字节长度
//                            tnum ++;
//                        }
//                        else
//                        {
//                            // 清0重新接收
//                            tnum = 0;
//                            // 清空标志位
//                            f_h1_flag = 0;
//                            f_h_flag = 0;
//                            f_fun_word = 0;
//                        }
//                    }
//                }
//                else						// 有帧头，无功能字，判断是否为有效功能字
//                {
//                    if ((chrtmp[tnum] == FunWord_WF) || chrtmp[tnum] == FunWord_SM)
//                    {
//                        f_fun_word = chrtmp[tnum];//记录功能字
//                        tnum ++;
//                    }
//                    else
//                    {
//                        // 清0重新接收
//                        tnum = 0;
//                        // 清空标志位
//                        f_h1_flag = 0;
//                        f_h_flag = 0;
//                    }
//                }
//            }
//            else						// 没有接收到帧头
//            {
//                if (f_h1_flag == 1)			        //没有帧头，有帧头1。下一步判断是否为第2个字节
//                {
//                    if (chrtmp[tnum] == Frame_Header2)          // 如果为帧头的第2个字节，接收到帧头标志位标志位置1，tnum自增
//                    {
//                        f_h_flag = 1;
//                        tnum ++;
//                    }
//                    else
//                    {
//                        // 这里再添加一个判断，出现 3A 3A 3B xx的情况，如果没有这个判断会重新计数，导致丢帧
//                        if(chrtmp[tnum] == Frame_Header1){
//                            f_h1_flag = 1;
//                            tnum = 1;
//                        }else{
//                            // 重新计数，但如果出现 3A 3A 3B xx的情况，会导致丢帧，要加上上面的判断
//                            f_h1_flag = 0;
//                            tnum = 0;
//                        }
//                    }
//                }
//                else						//没有帧头，没有有帧头1。下一步判断，是否为帧头的第1个字节
//                {
//                    if (chrtmp[tnum] == Frame_Header1)  // 如果为帧头的第1个字节，标志位置1，tnum自增
//                    {
//                        f_h1_flag = 1;
//                        tnum ++;
//                    }
//                    else                                // 否则，标志位清0，tnum清0
//                    {
//                        tnum = 0;
//                    }
//                }
//            }

//            static int xx=0;
//            // 判断多长的数据没有换行符，如果超过2000，会人为向数据接收区添加换行，来保证CPU占用率不会过高，不会导致卡顿
//            // 但由于是先插入换行，后插入接收到的数据，所以每一箩数据并不是2000
//            if(chrtmp[tnum] != 0x0A){
//                ++xx;
//                if(xx > 2000){
////                    ui->txtRec->appendPlainText("");
////                    ui->txtRec->appendPlainText("");
//                    xx=0;
//                }
//            }else{
//                xx=0;
//            }

//            // 大于MaxFrameLength个字节的帧不接收
//            if (tnum > (MaxFrameLength - 1) )
//            {
//                tnum = 0;
//                f_h1_flag = 0;
//                f_h_flag = 0;
//                f_t1_flag = 0;
//                //f_fun_word = 0;
//                //f_length = 0;
//                continue;
//            }
//        }
//    }
}

/*
// 适用于有帧头帧尾、无功能字和有效字段长度的接收
void MainWindow::xFrameDataFilter(QByteArray *str)
{
    int num = str->size();
    if(num)
    {
        for(int i=0; i<num; i++)
        {
            chrtmp[tnum] = str->at(i);  		// 从接收缓存区读取一个字节
            if (f_h_flag == 1)  // 有帧头，判断帧尾，接收消息
            {
                if (f_t1_flag == 1) //有帧头，有帧尾1
                {
                    if (chrtmp[tnum] == Frame_Tail2)
                    {
                        tnum ++;

                        // 接收到一帧有效帧
                        // 用户处理代码 //
                        // 根据接收到的数量，合成帧字段
                        // 调试信息输出，显示有效帧的内容（16进制显示）
                        if(!ui->widget_5->isHidden()){
                            QByteArray str1;
                            for(int i=0; i<tnum; i++)
                            {
                                str1.append(chrtmp[i]);
                            }
                            //ui->txtFrameEffective->appendPlainText(str1.toHex().toUpper());
                            str1 = str1.toHex().toUpper();
                            QString str2;
                            for(int i = 0; i<str1.length (); i+=2)
                            {
                                str2 += str1.mid (i,2);
                                str2 += " ";
                            }
                            ui->txtFrameEffective->appendPlainText(str2);
                        }

                        //  处理完用户代码，重新接收计数 //
                        tnum = 0;
                        // 清空标志位，之前一直忘了
                        f_h1_flag = 0;
                        f_h_flag = 0;
                        f_t1_flag = 0;

                        // 将接收到符合帧定义的帧，原路发送回去 //
                        //return str1;
                        //ui->lineEdit->setText(str1.toHex().toUpper());

                    }
                    else
                    {
                        f_t1_flag = 0;
                        tnum ++;
                    }
                }
                else						// 有帧头，无帧尾1
                {
                    if (chrtmp[tnum] == Frame_Tail1)
                    {
                        f_t1_flag = 1;
                        tnum ++;
                    }
                    else					// 接收消息包中间内容
                    {
                        tnum ++;
                    }
                }
            }
            else						// 没有接收到帧头
            {
                if (f_h1_flag == 1)			        //没有帧头，有帧头1。下一步判断是否为第2个字节
                {
                    if (chrtmp[tnum] == Frame_Header2)          // 如果为帧头的第2个字节，接收到帧头标志位标志位置1，tnum自增
                    {
                        f_h_flag = 1;
                        tnum ++;
                    }
                    else
                    {
                        // 这里再添加一个判断，出现 3A 3A 3B xx的情况，如果没有这个判断会重新计数，导致丢帧
                        if(chrtmp[tnum] == Frame_Header1){
                            f_h1_flag = 1;
                            tnum = 1;
                        }else{
                            // 重新计数，但如果出现 3A 3A 3B xx的情况，会导致丢帧，要加上上面的判断
                            f_h1_flag = 0;
                            tnum = 0;
                        }
                    }
                }
                else						//没有帧头，没有有帧头1。下一步判断，是否为帧头的第1个字节
                {
                    if (chrtmp[tnum] == Frame_Header1)  // 如果为帧头的第1个字节，标志位置1，tnum自增
                    {
                        f_h1_flag = 1;
                        tnum ++;
                    }
                    else                                // 否则，标志位清0，tnum清0
                    {
                        tnum = 0;
                    }
                }
            }

            // 大于MaxFrameLength个字节的帧不接收
            if (tnum > (MaxFrameLength - 1) )
            {
                tnum = 0;
                f_h1_flag = 0;
                f_h_flag = 0;
                f_t1_flag = 0;
                continue;
            }
        }
    }
}*/

//void MainWindow::on_btnFramaDebug_clicked()
//{
//    if(ui->widget_5->isHidden()){
//        ui->widget_5->show();
//        ui->frame_3->show();
//    }else{
//        ui->widget_5->hide();
//        ui->frame_3->hide();
//    }
//}

// 绘图图表初始化
void MainWindow::QPlot_init(QCustomPlot *customPlot)
{
    // 添加曲线名称
    QStringList lineNames;//设置图例的文本
    lineNames << "U相" << "V相" << "W相";

    // 曲线初始颜色
    QColor initColor[3] = {QColor(0,146,152), QColor(162,0,124), QColor(241,175,0)};//QColor(255,255,255)};//白色

    // 图表添加3条曲线，并设置初始颜色，和图例名称
    for(int i=0; i<plotCount; i++){
        pCurve[i] = customPlot->addGraph();
        pCurve[i]->setPen(QPen(QColor(initColor[i])));
        pCurve[i]->setName(lineNames.at(i));
    }

    // 设置背景颜色
    customPlot->setBackground(QColor(255,255,255));

    // 设置背景选择框颜色
    ui->btnColourBack->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(QColor(255,255,255).name()));

    // 曲线选择框颜色，与曲线同步颜色。这样写太复杂了，用控件指针数组在下面写过了，记得要在addGraph()之后才有效。
    //ui->btnColourCurve1->setStyleSheet("border:0px solid;background-color:rgb(0,146,152)");
    //ui->btnColourCurve1->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(initColor[0].name()));
    //ui->btnColourCurve20->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(pCurve[]->pen().color().name()));

    // 设置坐标轴名称
    customPlot->xAxis->setLabel("三相电压");
    customPlot->yAxis->setLabel("");

    // 设置x,y坐标轴显示范围
    pointCountX = ui->txtPointCountX->text().toUInt();
    pointCountY = ui->txtPointCountY->text().toUInt();
    customPlot->xAxis->setRange(0,pointCountX);
    customPlot->yAxis->setRange(pointCountY/2*-1,pointCountY/2);

    //customPlot->axisRect()->setupFullAxesBox();//四边安装轴并显示
    //customPlot->xAxis->ticker()->setTickOrigin(1);//改变刻度原点为1
    //customPlot->xAxis->setNumberFormat("gbc");//g灵活的格式,b漂亮的指数形式，c乘号改成×
    //customPlot->xAxis->setNumberPrecision(1);//精度1
    customPlot->xAxis->ticker()->setTickCount(ui->txtMainScaleNumX->text().toUInt());//11个主刻度
    customPlot->yAxis->ticker()->setTickCount(ui->txtMainScaleNumY->text().toUInt());//11个主刻度
    customPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);//可读性优于设置
    customPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);//可读性优于设置

    // 显示图表的图例
    customPlot->legend->setVisible(true);

    // 设置波形曲线的复选框字体颜色
    //ui->chkVisibleCurve1->setStyleSheet("QCheckBox{color:rgb(255,0,0)}");//设定前景颜色,就是字体颜色

    // 允许用户用鼠标拖动轴范围，以鼠标为中心滚轮缩放，点击选择图形:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    // 设置鼠标滚轮的缩放倍率，如果不设置默认为0.85，大于1反方向缩放
    //customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮缩放的轴方向，仅设置垂直轴。垂直轴和水平轴全选使用：Qt::Vertical | Qt::Horizontal
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
}

// 绘图图表的设置控件初始化，主要用于关联控件的信号槽
void MainWindow::QPlot_widget_init(void)
{
    // 获取控件指针数组，方便设置时编码书写

    pChkVisibleCurve[0] = ui->chkVisibleCurve1; pBtnColourCurve[0] = ui->btnColourCurve1; pTxtValueCurve[0] = ui->txtValueCurve1; pRdoBoldCurve[0] = ui->rdoBoldCurve1;
    pChkVisibleCurve[1] = ui->chkVisibleCurve2; pBtnColourCurve[1] = ui->btnColourCurve2; pTxtValueCurve[1] = ui->txtValueCurve2; pRdoBoldCurve[1] = ui->rdoBoldCurve2;
    pChkVisibleCurve[2] = ui->chkVisibleCurve3; pBtnColourCurve[2] = ui->btnColourCurve3; pTxtValueCurve[2] = ui->txtValueCurve3; pRdoBoldCurve[2] = ui->rdoBoldCurve3;

    pCmbLineStyle[0] = ui->cmbLineStyle1; pCmbScatterStyle[0] = ui->cmbScatterStyle1;
    pCmbLineStyle[1] = ui->cmbLineStyle2; pCmbScatterStyle[1] = ui->cmbScatterStyle2;
    pCmbLineStyle[2] = ui->cmbLineStyle3; pCmbScatterStyle[2] = ui->cmbScatterStyle3;


    // 设置颜色选择框的初始背景颜色，与曲线同步颜色
    for(int i=0; i<plotCount; i++){
        pBtnColourCurve[i]->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(QColor(pCurve[i]->pen().color()).name()));
    }

    // 可见性选择框关联
    for(int i=0; i<plotCount; i++){
        connect(pChkVisibleCurve[i], &QCheckBox::clicked, [=](){
            curveSetVisible(pPlot1, pCurve[i], pChkVisibleCurve[i]->checkState());
        });
    }

    // 颜色选择框关联
    for(int i=0; i<plotCount; i++){
        connect(pBtnColourCurve[i], &QPushButton::clicked, [=](){
            curveSetColor(pPlot1, pCurve[i], pBtnColourCurve[i]);
        });
    }

    // 加粗显示多选框关联。尽量别用，会导致CPU使用率升高
    for(int i=0; i<plotCount; i++){
        connect(pRdoBoldCurve[i], &QRadioButton::clicked, [=](){
            curveSetBold(pPlot1, pCurve[i], pRdoBoldCurve[i]->isChecked());
        });
    }

    // 曲线样式选择关联
    for(int i=0; i<plotCount; i++){
        connect(pCmbLineStyle[i], &QComboBox::currentTextChanged, [=](){
            curveSetLineStyle(pPlot1, pCurve[i], pCmbLineStyle[i]->currentIndex());
        });
    }

    // 散点样式选择关联
    for(int i=0; i<plotCount; i++){
        connect(pCmbScatterStyle[i], &QComboBox::currentTextChanged, [=](){
            curveSetScatterStyle(pPlot1, pCurve[i], pCmbScatterStyle[i]->currentIndex()+1);});
        pCmbScatterStyle[i]->setIconSize(QSize(25,17)); // 设置图片显示像素大小，不然会默认大小显示会模糊;
    }



    //QIcon ssCircleIcon (":/pic/ssCircle.png");
    //ui->cmbScatterStyle1->addItem(ssCircleIcon,"空心圆");
    //默认空心
//    for(int i=0; i<3; i++){
//        pCurve[i]->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)4, 5));
//    }



}

// 定时器溢出处理槽函数。用来生成曲线的坐标数据。
void MainWindow::TimeData_Update(void)
{
//    // 生成坐标数据
//    static float f;
//    f += 0.01;
//    //qDebug() << sin(f)*100;
//    // 将坐标数据，传递给曲线
////    ShowPlot_TimeDemo(pPlot1, sin(f)*100);
//    ShowPlot_TimeDemo(pPlot1, f);


    if(datavalid){
        ShowPlot_WaveForm(pPlot1, (float*)chrtmp);
        datavalid = false;
    }

}

// 曲线更新绘图，定时器绘图演示
void MainWindow::ShowPlot_TimeDemo(QCustomPlot *customPlot, double num)
{
    cnt++;
    // 给曲线添加数据
//    for(int i=0; i<plotCount; i++){
//        pTxtValueCurve[i]->setText(QString::number(num-i*10,'g',8));// 显示曲线当前值
//        pCurve[i]->addData(cnt, num-i*10);
//    }

    pCurve[0]->addData(cnt, sin(num)*100);
    pCurve[1]->addData(cnt, cos(num)*100);
    pCurve[2]->addData(cnt, tan(num)*100);



//    for(int i=10; i<plotCount; i++){
//        pTxtValueCurve[i]->setText(QString::number(num+(i-9)*10,'g',8));// 显示曲线当前值
//        pCurve[i]->addData(cnt, num+(i-9)*10);
//    }

    // 设置x坐标轴显示范围，使其自适应缩放x轴，x轴最大显示pointCountX个点。与chkTrackX复选框有关
    if(ui->chkTrackX->checkState()){
        //customPlot->xAxis->setRange((pCurve[0]->dataCount()>pointCountX)?(pCurve[0]->dataCount()-pointCountX):0, pCurve[0]->dataCount());
        setAutoTrackX(customPlot);
    }
    // 设置y坐标轴显示范围，使其自适应曲线缩放
    if(ui->chkAdjustY->checkState()){
        setAutoTrackY(customPlot);
    }

    // 更新绘图，这种方式在高填充下太浪费资源。有另一种方式rpQueuedReplot，可避免重复绘图。
    // 最好的方法还是将数据填充、和更新绘图分隔开。将更新绘图单独用定时器更新。例程数据量较少没用单独定时器更新，实际工程中建议大家加上。
    //customPlot->replot();
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
    ////计算帧数
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 1) // 每1秒求一次平均值
    {
        //状态栏显示帧数和数据总数
        ui->statusbar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size())
            , 0);
        lastFpsKey = key;
        frameCount = 0;
    }

}

// 曲线更新绘图，波形数据绘图
void MainWindow::ShowPlot_WaveForm(QCustomPlot *customPlot, float value[])
{
    cnt++;
    // 给曲线添加数据
    for(int i=0; i<3; i++){
        //QString strNum = QString::number(num,'g',8);// double类型
        pTxtValueCurve[i]->setText(QString::number(value[i],'g',5));// 显示曲线当前值
        pCurve[i]->addData(cnt, value[i]);// 从原值获取数据

        //pCurve[i]->addData(cnt, pTxtValueCurve[i]->text().toShort());// 从输入框获取数据
        // 因为20条线重叠在一起，所以QCustomPlot输入为0时看起来像不显示，隐藏其他后观察单条曲线是可以看到显示的
    }

    // 设置x坐标轴显示范围，使其自适应缩放x轴，x轴最大显示pointCountX个点。与chkTrackX复选框有关
    if(ui->chkTrackX->checkState()){
        //customPlot->xAxis->setRange((pCurve[0]->dataCount()>pointCountX)?(pCurve[0]->dataCount()-pointCountX):0, pCurve[0]->dataCount());
        setAutoTrackX(customPlot);
    }
    // 设置y坐标轴显示范围，使其自适应曲线缩放
    if(ui->chkAdjustY->checkState()){
        setAutoTrackY(customPlot);
    }

    // 更新绘图，这种方式在高填充下太浪费资源。有另一种方式rpQueuedReplot，可避免重复绘图。
    // 最好的方法还是将数据填充、和更新绘图分隔开。将更新绘图单独用定时器更新。例程数据量较少没用单独定时器更新，实际工程中建议大家加上。
    //customPlot->replot();
    customPlot->replot(QCustomPlot::rpQueuedReplot);







//    static QTime time(QTime::currentTime());
//    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
//    ////计算帧数
//    static double lastFpsKey;
//    static int frameCount;
//    ++frameCount;
//    if (key-lastFpsKey > 1) // 每1秒求一次平均值
//    {
//        //状态栏显示帧数和数据总数
//        ui->statusbar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//            .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size())
//            , 0);
//        lastFpsKey = key;
//        frameCount = 0;
//    }

}

/* 功能：隐藏/显示曲线n
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线的可见性，>0可见，0不可见
 * */
void MainWindow::curveSetVisible(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    if(arg1){
        pCurve->setVisible(true);
    }else{
        pCurve->setVisible(false);
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：弹出颜色对话框，设置曲线n的颜色
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * QPushButton *btn：曲线颜色选择框的按键，与曲线的颜色同步
 * */
void MainWindow::curveSetColor(QCustomPlot *pPlot, QCPGraph *pCurve, QPushButton *btn)
{
    // 获取当前颜色
    QColor bgColor(0,0,0);
    //bgColor = btn->palette().color(QPalette::Background);// 由pushButton的背景色获得颜色
    bgColor = pCurve->pen().color();// 由curve曲线获得颜色
    // 以当前颜色打开调色板，父对象，标题，颜色对话框设置项（显示Alpha透明度通道）
    //QColor color = QColorDialog::getColor(bgColor);
    QColor color = QColorDialog::getColor(bgColor, this,
                                     tr("颜色对话框"),
                                     QColorDialog::ShowAlphaChannel);
    // 判断返回的颜色是否合法。若点击x关闭颜色对话框，会返回QColor(Invalid)无效值，直接使用会导致变为黑色。
    if(color.isValid()){
        // 设置选择框颜色
        btn->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(color.name()));
        // 设置曲线颜色
        QPen pen = pCurve->pen();
        pen.setBrush(color);
        pCurve->setPen(pen);
        //pCurve->setPen(QPen(color));
    }
    // 更新绘图
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：加粗显示曲线n
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线的粗细，>0粗，0细
 * */
void MainWindow::curveSetBold(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 预先读取曲线的颜色
    QPen pen = pCurve->pen();
    //pen.setBrush(pCurve->pen().color());// 由curve曲线获得颜色

    if(arg1){
        pen.setWidth(3);
        pCurve->setPen(pen);
    }else{
        pen.setWidth(1);
        pCurve->setPen(pen);
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：选择曲线样式（线，点，积）
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线样式（线，点，积）
 * */
void MainWindow::curveSetLineStyle(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 设置曲线样式
    //customPlot->graph(19)->setLineStyle(QCPGraph::lsLine); // 数据点通过直线连接
    //customPlot->graph(19)->setLineStyle((QCPGraph::LineStyle)i);//设置线性
    //pCurve->setLineStyle(QCPGraph::LineStyle(arg1));
    pCurve->setLineStyle((QCPGraph::LineStyle)arg1);
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：选择散点样式（空心圆、实心圆、正三角、倒三角）
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：散点样式（空心圆、实心圆、正三角、倒三角）
 * */
void MainWindow::curveSetScatterStyle(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 设置散点样式
    //customPlot->graph(19)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5)); // 空心圆
    //pCurve->setScatterStyle(QCPScatterStyle::ScatterShape(arg1)); // 散点样式
    //pCurve->setScatterStyle((QCPScatterStyle::ScatterShape)arg1); // 散点样式
    if(arg1 <= 10){
        pCurve->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)arg1, 5)); // 散点样式
    }else{ // 后面的散点图形略复杂，太小会看不清
        pCurve->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)arg1, 8)); // 散点样式
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

// 图例显示与否
void MainWindow::on_chkShowLegend_stateChanged(int arg1)
{
    if(arg1){
        // 显示图表的图例
        pPlot1->legend->setVisible(true);
    }else{
        // 不显示图表的图例
        pPlot1->legend->setVisible(false);
    }
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

// 绘图演示-曲线
//void MainWindow::on_chkDrawDemo_stateChanged(int arg1)
//{
//    if(arg1){
//        timer->start(10);
//    }else{
//        timer->stop();
//    }
//}

// 设置曲线x轴自动跟随
void MainWindow::setAutoTrackX(QCustomPlot *pPlot)
{

    pointCountX = ui->txtPointCountX->text().toUInt();
    if(pCurve[0]->dataCount() < pointCountX){
        pPlot->xAxis->setRange(0,pointCountX);
    }else{
        pPlot->xAxis->setRange((pCurve[0]->dataCount()>pointCountX)?(pCurve[0]->dataCount()-pointCountX):0, pCurve[0]->dataCount());
    }
}

// 设置曲线x轴手动设置范围（依照右下角输入框）
void MainWindow::setManualSettingX(QCustomPlot *pPlot)
{
    pointOriginX = ui->txtPointOriginX->text().toInt();
    pointCountX = ui->txtPointCountX->text().toUInt();
    pPlot->xAxis->setRange(pointOriginX, pointOriginX+pointCountX);
}

// 设置Y轴自适应
void MainWindow::setAutoTrackY(QCustomPlot *pPlot)
{
    pPlot->graph(0)->rescaleValueAxis();// y轴自适应，可放大可缩小
    for(int i=1; i<plotCount; i++){
        pPlot->graph(i)->rescaleValueAxis(true);// y轴自适应，只能放大
    }
}

// 重新设置X轴显示的点数
void MainWindow::on_txtPointCountX_returnPressed()
{
    if(ui->chkTrackX->checkState()){
        setAutoTrackX(pPlot1);
    }else{
        setManualSettingX(pPlot1);
    }
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_txtPointCountY_returnPressed()
{
    pointCountY = ui->txtPointCountY->text().toUInt();
    pPlot1->yAxis->setRange(pointCountY/2*-1,pointCountY/2);
    ui->txtPointOriginY->setText(QString::number(pointCountY/2*-1));
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_btnColourBack_clicked()
{
    // 获取当前颜色
    QColor bgColor(0,0,0);
    bgColor = ui->btnColourBack->palette().color(QPalette::Background);// 由pushButton的背景色获得颜色
    // 以当前颜色打开调色板，父对象，标题，颜色对话框设置项（显示Alpha透明度通道）
    //QColor color = QColorDialog::getColor(bgColor);
    QColor color = QColorDialog::getColor(bgColor, this,
                                     tr("颜色对话框"),
                                     QColorDialog::ShowAlphaChannel);

    // 判断返回的颜色是否合法。若点击x关闭颜色对话框，会返回QColor(Invalid)无效值，直接使用会导致变为黑色。
    if(color.isValid()){
        // 设置背景颜色
        pPlot1->setBackground(color);
        // 设置背景选择框颜色
        ui->btnColourBack->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(color.name()));
    }
    // 更新绘图
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_txtPointOriginX_returnPressed()
{
    setManualSettingX(pPlot1);
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_chkTrackX_stateChanged(int arg1)
{
    if(arg1){
        ui->txtPointOriginX->setEnabled(false);
        setAutoTrackX(pPlot1);
        pPlot1->replot(QCustomPlot::rpQueuedReplot);
    }else{
        ui->txtPointOriginX->setEnabled(true);
    }
}

void MainWindow::on_chkAdjustY_stateChanged(int arg1)
{
    if(arg1){
        ui->txtPointOriginY->setEnabled(false);
        ui->txtPointCountY->setEnabled(false);
        setAutoTrackY(pPlot1);
        pPlot1->replot(QCustomPlot::rpQueuedReplot);
    }else{
        ui->txtPointOriginY->setEnabled(true);
        ui->txtPointCountY->setEnabled(true);
    }
}

void MainWindow::on_txtPointOriginY_returnPressed()
{
    pointOriginY = ui->txtPointOriginY->text().toInt();
    pointCountY = ui->txtPointCountY->text().toUInt();
    pPlot1->yAxis->setRange(pointOriginY, pointOriginY+pointCountY);
    qDebug() << pointOriginY << pointCountY;
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

// 每次图表重绘后，都会更新当前显示的原点坐标与范围。与上次不同时才会更新显示，解决有曲线数据时无法输入y的参数的问题
void MainWindow::repPlotCoordinate()
{
    static int xOrigin, yOrigin, yCount;
    static int xOriginLast, yOriginLast, yCountLast;

    xOrigin = pPlot1->xAxis->range().lower;
    yOrigin = pPlot1->yAxis->range().lower;
    yCount = pPlot1->yAxis->range().size();
    // 与上次不同时才会更新显示，解决有曲线数据时无法输入y的参数的问题
    if(xOriginLast != xOrigin){
        ui->txtPointOriginX->setText(QString::number(xOrigin));
    }
    if(yOriginLast != yOrigin){
        ui->txtPointOriginY->setText(QString::number(yOrigin));
    }
    if(yCountLast != yCount){
        ui->txtPointCountY->setText(QString::number(yCount));
    }
    // 记录历史值
    xOriginLast = xOrigin;
    yOriginLast = yOrigin;
    yCountLast = yCount;
}

// 清空绘图
void MainWindow::on_btnClearGraphs_clicked()
{
    //pPlot1->clearGraphs(); // 清除图表的所有数据和设置，需要重新设置才能重新绘图
    //pPlot1->clearPlottables(); // 清除图表中所有曲线，需要重新添加曲线才能绘图
    for(int i=0; i<plotCount; i++){
        pPlot1->graph(i)->data().data()->clear(); // 仅仅清除曲线的数据
    }
    cnt = 0;
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

// 设置X轴主刻度个数
void MainWindow::on_txtMainScaleNumX_returnPressed()
{
    pPlot1->xAxis->ticker()->setTickCount(ui->txtMainScaleNumX->text().toUInt());
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}

// 设置Y轴主刻度个数
void MainWindow::on_txtMainScaleNumY_returnPressed()
{
    pPlot1->yAxis->ticker()->setTickCount(ui->txtMainScaleNumY->text().toUInt());
    pPlot1->replot(QCustomPlot::rpQueuedReplot);
}















































































// 绘图图表初始化
void MainWindow::QPlot_init1(QCustomPlot *customPlot)
{
    // 添加曲线名称
    QStringList lineNames;//设置图例的文本
    lineNames << "P" << "I" << "D";
    // 曲线初始颜色
    QColor initColor[3] = {QColor(0,146,152), QColor(241,0,4), QColor(241,175,0)};//QColor(255,255,255)};//白色
    // 图表添加3条曲线，并设置初始颜色，和图例名称

        for(int i=0; i<plotCount; i++){
            pCurve1[i] = customPlot->addGraph();
            pCurve1[i]->setPen(QPen(QColor(initColor[i])));
            pCurve1[i]->setName(lineNames.at(i));
        }

    // 设置背景颜色
    customPlot->setBackground(QColor(255,255,255));
    // 设置背景选择框颜色
    ui->btnColourBack_2->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(QColor(255,255,255).name()));

    // 曲线选择框颜色，与曲线同步颜色。这样写太复杂了，用控件指针数组在下面写过了，记得要在addGraph()之后才有效。
    //ui->btnColourCurve1->setStyleSheet("border:0px solid;background-color:rgb(0,146,152)");
    //ui->btnColourCurve1->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(initColor[0].name()));
    //ui->btnColourCurve20->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(pCurve[]->pen().color().name()));

    // 设置坐标轴名称
    customPlot->xAxis->setLabel("PID");
    customPlot->yAxis->setLabel("");

    // 设置x,y坐标轴显示范围
    pointCountX1 = ui->txtPointCountX_2->text().toUInt();
    pointCountY1 = ui->txtPointCountY_2->text().toUInt();
    customPlot->xAxis->setRange(0,pointCountX1);
    customPlot->yAxis->setRange(pointCountY1/2*-1,pointCountY1/2);

    //customPlot->axisRect()->setupFullAxesBox();//四边安装轴并显示
    //customPlot->xAxis->ticker()->setTickOrigin(1);//改变刻度原点为1
    //customPlot->xAxis->setNumberFormat("gbc");//g灵活的格式,b漂亮的指数形式，c乘号改成×
    //customPlot->xAxis->setNumberPrecision(1);//精度1
    customPlot->xAxis->ticker()->setTickCount(ui->txtMainScaleNumX_2->text().toUInt());//11个主刻度
    customPlot->yAxis->ticker()->setTickCount(ui->txtMainScaleNumY_2->text().toUInt());//11个主刻度
    customPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);//可读性优于设置
    customPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);//可读性优于设置

    // 显示图表的图例
    customPlot->legend->setVisible(true);

    // 设置波形曲线的复选框字体颜色
    //ui->chkVisibleCurve1->setStyleSheet("QCheckBox{color:rgb(255,0,0)}");//设定前景颜色,就是字体颜色

    // 允许用户用鼠标拖动轴范围，以鼠标为中心滚轮缩放，点击选择图形:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    // 设置鼠标滚轮的缩放倍率，如果不设置默认为0.85，大于1反方向缩放
    //customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮缩放的轴方向，仅设置垂直轴。垂直轴和水平轴全选使用：Qt::Vertical | Qt::Horizontal
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
}

// 绘图图表的设置控件初始化，主要用于关联控件的信号槽
void MainWindow::QPlot_widget_init1(void)
{
    // 获取控件指针数组，方便设置时编码书写
    pChkVisibleCurve1[0] = ui->chkVisibleCurve1_2; pBtnColourCurve1[0] = ui->btnColourCurve1_2; pTxtValueCurve1[0] = ui->txtValueCurve1_2; pRdoBoldCurve1[0] = ui->rdoBoldCurve1_2;
    pChkVisibleCurve1[1] = ui->chkVisibleCurve1_4; pBtnColourCurve1[1] = ui->btnColourCurve1_4; pTxtValueCurve1[1] = ui->txtValueCurve1_4; pRdoBoldCurve1[1] = ui->rdoBoldCurve1_4;
    pChkVisibleCurve1[2] = ui->chkVisibleCurve1_5; pBtnColourCurve1[2] = ui->btnColourCurve1_5; pTxtValueCurve1[2] = ui->txtValueCurve1_5; pRdoBoldCurve1[2] = ui->rdoBoldCurve1_5;

    pCmbLineStyle1[0] = ui->cmbLineStyle1_2; pCmbScatterStyle1[0] = ui->cmbScatterStyle1_2;
    pCmbLineStyle1[1] = ui->cmbLineStyle1_4; pCmbScatterStyle1[1] = ui->cmbScatterStyle1_4;
    pCmbLineStyle1[2] = ui->cmbLineStyle1_5; pCmbScatterStyle1[2] = ui->cmbScatterStyle1_5;




    // 设置颜色选择框的初始背景颜色，与曲线同步颜色
    for(int i=0; i<plotCount; i++){
        pBtnColourCurve1[i]->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(QColor(pCurve1[i]->pen().color()).name()));
    }



    // 可见性选择框关联
    for(int i=0; i<plotCount; i++){
        connect(pChkVisibleCurve1[i], &QCheckBox::clicked, [=](){
            curveSetVisible_2(pPlot2, pCurve1[i], pChkVisibleCurve1[i]->checkState());
        });
    }

    // 颜色选择框关联
    for(int i=0; i<plotCount; i++){
        connect(pBtnColourCurve1[i], &QPushButton::clicked, [=](){
            curveSetColor_2(pPlot2, pCurve1[i], pBtnColourCurve1[i]);
        });
    }

    // 加粗显示多选框关联。尽量别用，会导致CPU使用率升高
    for(int i=0; i<plotCount; i++){
        connect(pRdoBoldCurve1[i], &QRadioButton::clicked, [=](){
            curveSetBold_2(pPlot2, pCurve1[i], pRdoBoldCurve1[i]->isChecked());
        });
    }

    // 曲线样式选择关联
    for(int i=0; i<plotCount; i++){
        connect(pCmbLineStyle1[i], &QComboBox::currentTextChanged, [=](){
            curveSetLineStyle_2(pPlot2, pCurve1[i], pCmbLineStyle1[i]->currentIndex());
        });
    }

    // 散点样式选择关联
    for(int i=0; i<plotCount; i++){
        connect(pCmbScatterStyle1[i], &QComboBox::currentTextChanged, [=](){
            curveSetScatterStyle_2(pPlot2, pCurve1[i], pCmbScatterStyle1[i]->currentIndex()+1);});
        pCmbScatterStyle1[i]->setIconSize(QSize(25,17)); // 设置图片显示像素大小，不然会默认大小显示会模糊;
    }



    //QIcon ssCircleIcon (":/pic/ssCircle.png");
    //ui->cmbScatterStyle1->addItem(ssCircleIcon,"空心圆");
    //默认空心
//    for(int i=0; i<3; i++){
//        pCurve1[i]->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)4, 5));
//    }

}

// 定时器溢出处理槽函数。用来生成曲线的坐标数据。
void MainWindow::TimeData_Update1(void)
{
//    // 生成坐标数据
//    static float f;
//    f += 0.01;
//    //qDebug() << sin(f)*100;
//    // 将坐标数据，传递给曲线
//    ShowPlot_TimeDemo(pPlot2, sin(f)*100);
    if(datavalid1)
    {
        ShowPlot_WaveForm1(pPlot2, (float*)&chrtmp[12]);
        datavalid1 = false;
    }

}

// 曲线更新绘图，定时器绘图演示
void MainWindow::ShowPlot_TimeDemo1(QCustomPlot *customPlot, double num)
{
    cnt1++;
    // 给曲线添加数据
    for(int i=0; i<plotCount; i++){
        pTxtValueCurve1[i]->setText(QString::number(num-i*10,'g',8));// 显示曲线当前值
        pCurve1[i]->addData(cnt1, num-i*10);
    }
    for(int i=10; i<plotCount; i++){
        pTxtValueCurve1[i]->setText(QString::number(num+(i-9)*10,'g',8));// 显示曲线当前值
        pCurve1[i]->addData(cnt1, num+(i-9)*10);
    }

    // 设置x坐标轴显示范围，使其自适应缩放x轴，x轴最大显示pointCountX个点。与chkTrackX复选框有关
    if(ui->chkTrackX_2->checkState()){
        //customPlot->xAxis->setRange((pCurve[0]->dataCount()>pointCountX)?(pCurve[0]->dataCount()-pointCountX):0, pCurve[0]->dataCount());
        setAutoTrackX(customPlot);
    }
    // 设置y坐标轴显示范围，使其自适应曲线缩放
    if(ui->chkAdjustY_2->checkState()){
        setAutoTrackY(customPlot);
    }

    // 更新绘图，这种方式在高填充下太浪费资源。有另一种方式rpQueuedReplot，可避免重复绘图。
    // 最好的方法还是将数据填充、和更新绘图分隔开。将更新绘图单独用定时器更新。例程数据量较少没用单独定时器更新，实际工程中建议大家加上。
    //customPlot->replot();
    customPlot->replot(QCustomPlot::rpQueuedReplot);

//    static QTime time(QTime::currentTime());
//    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
//    ////计算帧数
//    static double lastFpsKey;
//    static int frameCount;
//    ++frameCount;
//    if (key-lastFpsKey > 1) // 每1秒求一次平均值
//    {
//        //状态栏显示帧数和数据总数
//        ui->statusbar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//            .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size())
//            , 0);
//        lastFpsKey = key;
//        frameCount = 0;
//    }

}

// 曲线更新绘图，波形数据绘图
void MainWindow::ShowPlot_WaveForm1(QCustomPlot *customPlot, float value[])
{
    cnt1++;
    // 给曲线添加数据
    for(int i=0; i<2; i++){
        //QString strNum = QString::number(num,'g',8);// double类型
        pTxtValueCurve1[i]->setText(QString::number(value[i]));// 显示曲线当前值
        pCurve1[i]->addData(cnt1, value[i]);// 从原值获取数据
        //pCurve[i]->addData(cnt, pTxtValueCurve[i]->text().toShort());// 从输入框获取数据
        // 因为20条线重叠在一起，所以QCustomPlot输入为0时看起来像不显示，隐藏其他后观察单条曲线是可以看到显示的
    }

    // 设置x坐标轴显示范围，使其自适应缩放x轴，x轴最大显示pointCountX个点。与chkTrackX复选框有关
    if(ui->chkTrackX_2->checkState()){
        //customPlot->xAxis->setRange((pCurve[0]->dataCount()>pointCountX)?(pCurve[0]->dataCount()-pointCountX):0, pCurve[0]->dataCount());
        setAutoTrackX_2(customPlot);
    }
    // 设置y坐标轴显示范围，使其自适应曲线缩放
    if(ui->chkAdjustY_2->checkState()){
        setAutoTrackY_2(customPlot);
    }

    // 更新绘图，这种方式在高填充下太浪费资源。有另一种方式rpQueuedReplot，可避免重复绘图。
    // 最好的方法还是将数据填充、和更新绘图分隔开。将更新绘图单独用定时器更新。例程数据量较少没用单独定时器更新，实际工程中建议大家加上。
    //customPlot->replot();
    customPlot->replot(QCustomPlot::rpQueuedReplot);

//    static QTime time(QTime::currentTime());
//    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
//    ////计算帧数
//    static double lastFpsKey;
//    static int frameCount;
//    ++frameCount;
//    if (key-lastFpsKey > 1) // 每1秒求一次平均值
//    {
//        //状态栏显示帧数和数据总数
//        ui->statusbar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//            .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size())
//            , 0);
//        lastFpsKey = key;
//        frameCount = 0;
//    }

}

/* 功能：隐藏/显示曲线n
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线的可见性，>0可见，0不可见
 * */
void MainWindow::curveSetVisible_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    if(arg1){
        pCurve->setVisible(true);
    }else{
        pCurve->setVisible(false);
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：弹出颜色对话框，设置曲线n的颜色
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * QPushButton *btn：曲线颜色选择框的按键，与曲线的颜色同步
 * */
void MainWindow::curveSetColor_2(QCustomPlot *pPlot, QCPGraph *pCurve, QPushButton *btn)
{
    // 获取当前颜色
    QColor bgColor(0,0,0);
    //bgColor = btn->palette().color(QPalette::Background);// 由pushButton的背景色获得颜色
    bgColor = pCurve->pen().color();// 由curve曲线获得颜色
    // 以当前颜色打开调色板，父对象，标题，颜色对话框设置项（显示Alpha透明度通道）
    //QColor color = QColorDialog::getColor(bgColor);
    QColor color = QColorDialog::getColor(bgColor, this,
                                     tr("颜色对话框"),
                                     QColorDialog::ShowAlphaChannel);
    // 判断返回的颜色是否合法。若点击x关闭颜色对话框，会返回QColor(Invalid)无效值，直接使用会导致变为黑色。
    if(color.isValid()){
        // 设置选择框颜色
        btn->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(color.name()));
        // 设置曲线颜色
        QPen pen = pCurve->pen();
        pen.setBrush(color);
        pCurve->setPen(pen);
        //pCurve->setPen(QPen(color));
    }
    // 更新绘图
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：加粗显示曲线n
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线的粗细，>0粗，0细
 * */
void MainWindow::curveSetBold_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 预先读取曲线的颜色
    QPen pen = pCurve->pen();
    //pen.setBrush(pCurve->pen().color());// 由curve曲线获得颜色

    if(arg1){
        pen.setWidth(3);
        pCurve->setPen(pen);
    }else{
        pen.setWidth(1);
        pCurve->setPen(pen);
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：选择曲线样式（线，点，积）
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：曲线样式（线，点，积）
 * */
void MainWindow::curveSetLineStyle_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 设置曲线样式
    //customPlot->graph(19)->setLineStyle(QCPGraph::lsLine); // 数据点通过直线连接
    //customPlot->graph(19)->setLineStyle((QCPGraph::LineStyle)i);//设置线性
    //pCurve->setLineStyle(QCPGraph::LineStyle(arg1));
    pCurve->setLineStyle((QCPGraph::LineStyle)arg1);
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

/* 功能：选择散点样式（空心圆、实心圆、正三角、倒三角）
 * QCustomPlot *pPlot：父控件，绘图图表
 * QCPGraph *pCurve：图表的曲线
 * int arg1：散点样式（空心圆、实心圆、正三角、倒三角）
 * */
void MainWindow::curveSetScatterStyle_2(QCustomPlot *pPlot, QCPGraph *pCurve, int arg1)
{
    // 设置散点样式
    //customPlot->graph(19)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5)); // 空心圆
    //pCurve->setScatterStyle(QCPScatterStyle::ScatterShape(arg1)); // 散点样式
    //pCurve->setScatterStyle((QCPScatterStyle::ScatterShape)arg1); // 散点样式
    if(arg1 <= 10){
        pCurve->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)arg1, 5)); // 散点样式
    }else{ // 后面的散点图形略复杂，太小会看不清
        pCurve->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)arg1, 8)); // 散点样式
    }
    pPlot->replot(QCustomPlot::rpQueuedReplot);
}

// 图例显示与否
void MainWindow::on_chkShowLegend_2_stateChanged(int arg1)
{
    if(arg1){
        // 显示图表的图例
        pPlot2->legend->setVisible(true);
    }else{
        // 不显示图表的图例
        pPlot2->legend->setVisible(false);
    }
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

// 绘图演示-曲线
//void MainWindow::on_chkDrawDemo_2_stateChanged(int arg1)
//{
//    if(arg1){
//        timer->start(10);
//    }else{
//        timer->stop();
//    }
//}

// 设置曲线x轴自动跟随
void MainWindow::setAutoTrackX_2(QCustomPlot *pPlot)
{
    pointCountX1 = ui->txtPointCountX_2->text().toUInt();
    if(pCurve1[0]->dataCount() < pointCountX1){
        pPlot->xAxis->setRange(0,pointCountX1);
    }else{
        pPlot->xAxis->setRange((pCurve1[0]->dataCount()>pointCountX)?(pCurve1[0]->dataCount()-pointCountX1):0, pCurve1[0]->dataCount());
    }
}

// 设置曲线x轴手动设置范围（依照右下角输入框）
void MainWindow::setManualSettingX_2(QCustomPlot *pPlot)
{
    pointOriginX1 = ui->txtPointOriginX_2->text().toInt();
    pointCountX1 = ui->txtPointCountX_2->text().toUInt();
    pPlot->xAxis->setRange(pointOriginX1, pointOriginX1+pointCountX1);
}

// 设置Y轴自适应
void MainWindow::setAutoTrackY_2(QCustomPlot *pPlot)
{
    pPlot->graph(0)->rescaleValueAxis();// y轴自适应，可放大可缩小
    for(int i=1; i<plotCount; i++){
        pPlot->graph(i)->rescaleValueAxis(true);// y轴自适应，只能放大
    }
}

// 重新设置X轴显示的点数
void MainWindow::on_txtPointCountX_2_returnPressed()
{
    if(ui->chkTrackX_2->checkState()){
        setAutoTrackX_2(pPlot2);
    }else{
        setManualSettingX_2(pPlot2);
    }
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_txtPointCountY_2_returnPressed()
{
    pointCountY1 = ui->txtPointCountY_2->text().toUInt();
    pPlot2->yAxis->setRange(pointCountY1/2*-1,pointCountY1/2);
    ui->txtPointOriginY_2->setText(QString::number(pointCountY1/2*-1));
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_btnColourBack_2_clicked()
{
    // 获取当前颜色
    QColor bgColor(0,0,0);
    bgColor = ui->btnColourBack_2->palette().color(QPalette::Background);// 由pushButton的背景色获得颜色
    // 以当前颜色打开调色板，父对象，标题，颜色对话框设置项（显示Alpha透明度通道）
    //QColor color = QColorDialog::getColor(bgColor);
    QColor color = QColorDialog::getColor(bgColor, this,
                                     tr("颜色对话框"),
                                     QColorDialog::ShowAlphaChannel);

    // 判断返回的颜色是否合法。若点击x关闭颜色对话框，会返回QColor(Invalid)无效值，直接使用会导致变为黑色。
    if(color.isValid()){
        // 设置背景颜色
        pPlot2->setBackground(color);
        // 设置背景选择框颜色
        ui->btnColourBack_2->setStyleSheet(QString("border:0px solid;background-color: %1;").arg(color.name()));
    }
    // 更新绘图
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_txtPointOriginX_2_returnPressed()
{
    setManualSettingX(pPlot2);
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::on_chkTrackX_2_stateChanged(int arg1)
{
    if(arg1){
        ui->txtPointOriginX_2->setEnabled(false);
        setAutoTrackX(pPlot2);
        pPlot1->replot(QCustomPlot::rpQueuedReplot);
    }else{
        ui->txtPointOriginX_2->setEnabled(true);
    }
}

void MainWindow::on_chkAdjustY_2_stateChanged(int arg1)
{
    if(arg1){
        ui->txtPointOriginY_2->setEnabled(false);
        ui->txtPointCountY_2->setEnabled(false);
        setAutoTrackY(pPlot2);
        pPlot2->replot(QCustomPlot::rpQueuedReplot);
    }else{
        ui->txtPointOriginY_2->setEnabled(true);
        ui->txtPointCountY_2->setEnabled(true);
    }
}

void MainWindow::on_txtPointOriginY_2_returnPressed()
{
    pointOriginY1 = ui->txtPointOriginY_2->text().toInt();
    pointCountY1 = ui->txtPointCountY_2->text().toUInt();
    pPlot2->yAxis->setRange(pointOriginY1, pointOriginY1+pointCountY1);
    qDebug() << pointOriginY1 << pointCountY1;
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

// 每次图表重绘后，都会更新当前显示的原点坐标与范围。与上次不同时才会更新显示，解决有曲线数据时无法输入y的参数的问题
void MainWindow::repPlotCoordinate_2()
{
    static int xOrigin, yOrigin, yCount;
    static int xOriginLast, yOriginLast, yCountLast;

    xOrigin = pPlot2->xAxis->range().lower;
    yOrigin = pPlot2->yAxis->range().lower;
    yCount = pPlot2->yAxis->range().size();
    // 与上次不同时才会更新显示，解决有曲线数据时无法输入y的参数的问题
    if(xOriginLast != xOrigin){
        ui->txtPointOriginX_2->setText(QString::number(xOrigin));
    }
    if(yOriginLast != yOrigin){
        ui->txtPointOriginY_2->setText(QString::number(yOrigin));
    }
    if(yCountLast != yCount){
        ui->txtPointCountY_2->setText(QString::number(yCount));
    }
    // 记录历史值
    xOriginLast = xOrigin;
    yOriginLast = yOrigin;
    yCountLast = yCount;
}

// 清空绘图
void MainWindow::on_btnClearGraphs_2_clicked()
{
    //pPlot1->clearGraphs(); // 清除图表的所有数据和设置，需要重新设置才能重新绘图
    //pPlot1->clearPlottables(); // 清除图表中所有曲线，需要重新添加曲线才能绘图
    for(int i=0; i<plotCount; i++){
        pPlot2->graph(i)->data().data()->clear(); // 仅仅清除曲线的数据
    }
    cnt1 = 0;
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

// 设置X轴主刻度个数
void MainWindow::on_txtMainScaleNumX_2_returnPressed()
{
    pPlot2->xAxis->ticker()->setTickCount(ui->txtMainScaleNumX_2->text().toUInt());
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}

// 设置Y轴主刻度个数
void MainWindow::on_txtMainScaleNumY_2_returnPressed()
{
    pPlot2->yAxis->ticker()->setTickCount(ui->txtMainScaleNumY_2->text().toUInt());
    pPlot2->replot(QCustomPlot::rpQueuedReplot);
}





















































































//MainWindow::PlotBaseWidget(QWidget *parent,int w,int h) :
//    QWidget(parent)
//{
//    setFixedSize(w,h);
//    InitParam();
//    m_plot = new QCustomPlot(this);
//    m_plot->setFixedSize(w,h);

//    //x坐标轴设置
//    m_plot->xAxis->setLabel("TIME");         //设置坐标名字
//    m_plot->xAxis->setLabelColor(Qt::black); //设置坐标颜色
//    m_plot->xAxis->setLabelPadding(1);       //设置坐标轴名称文本距离坐标轴刻度线距离
//    //m_plot->xAxis->setRange(0,1000);         //设置X轴范围


//    //设置Y轴
//    /*说明：虽然通过setVisible()可以设置Y2轴的不可见，但是在绘制时游标标签需要重新进行设置
//     *因为setVisible(false)后，Y2轴不绘制，Y2轴上的刻度线长度将无用，而将画笔设为Qt::NoPen，
//     *则是使用透明画笔绘制Y2轴，其刻度线长度仍然占用空间，也就不会将游标标签的空间压缩，导致游标
//     *标签显示不完整，这就需要在基础控件中修改游标标签的位置
//     */
//    m_plot->yAxis2->setTickLabels(false);     //设置y轴刻度值不显示
//    m_plot->yAxis2->setBasePen(Qt::NoPen);    //设置y2轴的绘制画笔
//    m_plot->yAxis2->setTickPen(Qt::NoPen);    //设置y2轴的主刻度线绘制画笔
//    m_plot->yAxis2->setSubTickPen(Qt::NoPen); //设置y2轴的子刻度线绘制画笔
//    connect(m_plot->yAxis2, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
//    m_plot->yAxis2->setVisible(true);
//    m_plot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(55); // add some padding to have space for tags
//    //m_plot->yAxis2->setRange(-2,2); //y轴的范围


//    //鼠标移动事件
//    connect(m_plot, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(slotShowValueTracer(QMouseEvent*)));
//    //设置曲线可拖拽、可缩放 //、可选择
//    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom ); //| QCP::iSelectPlottables
//    //m_plot->axisRect()->setRangeZoom(Qt::Horizontal);  //设置水平缩放
//    //m_plot->axisRect()->setRangeDrag(Qt::Horizontal);  //设置水平拖拽
//}


// 设置曲线信息
void MainWindow::CreateGraph(QMap<int,StLineInfo> mapLineInfo)
{
    m_mapLineInfo.clear();
    //显示图例
    pPlot1->legend->setVisible(true);
    //QColor bc = m_plot->legend->brush().color();
    pPlot1->legend->setBrush(QBrush(QColor(255,255,255,0)));
    pPlot1->legend->setRowSpacing(-3);
    // 创建图层及游标
    QMap<int,StLineInfo>::Iterator it = mapLineInfo.begin();
    for(int i = 0; it != mapLineInfo.end(); it++,i++)
    {
        StLineInfo st = it.value();
        StLineInfoAll line;
        // 创建图层
        line.graph = pPlot1->addGraph();
        line.graph->setName(st.name);
        //获取图层颜色
        QColor cc;
        if(st.c == Qt::transparent)
            cc = GetUsefullColor(i);
        else
            cc = st.c;

        line.graph->setPen(QPen(cc));

        pPlot1->legend->item(i)->setTextColor(cc);  //设置图例中每条线的文本颜色

        // 创建右侧游标
        line.tag = new PlotValueTracer(pPlot1,YAxisTracer);
        line.tag->setArrowLineLength(5);
        line.tag->setArrowHead(QCPLineEnding::esDisc);
        line.tag->setTracerStyle(QCPItemTracer::tsNone);
        line.tag->setArrowPen(line.graph->pen());
        line.tag->setLabelPen(Qt::NoPen);
        line.tag->setTextColor(cc);
        line.tag->setText("");

        // 创建数值游标
        line.vtrac = new PlotValueTracer(pPlot1,DataTracer);
        line.vtrac->setArrowHead(QCPLineEnding::esSpikeArrow);
        line.vtrac->setTracerStyle(QCPItemTracer::tsNone);
        line.vtrac->setArrowPen(QPen(cc));
//        line.vtrac->setTracerPen(QPen(cc));
        line.vtrac->setLabelPen(Qt::NoPen);
        line.vtrac->setTextColor(cc);


        line.info.name = st.name;
        line.info.c = cc;

        m_mapLineInfo.insert(it.key(),line);
    }
}
// 添加数据
void MainWindow::AddData(double key,QMap<int,double> mapData)
{
    QMap<int, double>::Iterator it = mapData.begin();
    for(; it != mapData.end(); it++)
    {
        double vv = it.value();

        QMap<int,StLineInfoAll>::Iterator ff = m_mapLineInfo.find(it.key());
        if(ff != m_mapLineInfo.end())
        {
            StLineInfoAll line = ff.value();
            line.graph->addData(key,vv);
            //更新标签位置和内容
            if(line.tag)
            {
                double graphValue = line.graph->dataMainValue(line.graph->dataCount()-1);
                line.tag->updateTracerPosition(key,graphValue);
                line.tag->setText(QString::number(graphValue, 'f', 2));
            }
        }
    }

    // make key axis range scroll with the data
    m_plot->xAxis->setRange(key, m_nXlength, Qt::AlignRight); //50是X轴的长度
    m_plot->replot();
}
// 设置游标标签的显示和隐藏
void MainWindow::ShowTagLabels(bool b)
{
    QMap<int,StLineInfoAll>::Iterator it = m_mapLineInfo.begin();  //曲线属性
    for(; it != m_mapLineInfo.end(); it++)
    {
        StLineInfoAll st = it.value();
        if(st.tag)
            st.tag->setVisible(b);
    }
    m_plot->replot();
}
// 设置数值游标的显示和隐藏
/*
void MainWindow::ShowValueTracer(bool b)
{
    QMap<int,StLineInfoAll>::Iterator it = m_mapLineInfo.begin();  //曲线属性
    for(; it != m_mapLineInfo.end(); it++)
    {
        StLineInfoAll st = it.value();
        if(st.vtrac)
            st.vtrac->setVisible(b);
    }
    m_plot->replot();
}*/
// 设置X轴范围
void MainWindow::SetXrange(int id,double lower, double upper)
{
    if(id == 0)
        pPlot1->xAxis->setRange(lower,upper);         //设置X轴范围
    else
        pPlot1->xAxis2->setRange(lower,upper);
}
// 设置Y轴范围
void MainWindow::SetYrange(int id,double lower, double upper)
{
    if(id == 0)
        pPlot1->yAxis->setRange(lower,upper);
    else
        pPlot1->yAxis2->setRange(lower,upper);
}

// 设置X轴窗口长度（动态曲线使用）
void MainWindow::SetXLength(int w)
{
    m_nXlength = w;
}



//显示数值游标
void MainWindow::slotShowValueTracer(QMouseEvent *event)
{
    double x = pPlot1->xAxis->pixelToCoord(event->pos().x());
    QMap<int,StLineInfoAll>::Iterator it = m_mapLineInfo.begin();
    for(; it != m_mapLineInfo.end();it++)
    {
        StLineInfoAll info = it.value();
        double y = 0;
        QSharedPointer<QCPGraphDataContainer> tmpContainer;
        tmpContainer = info.graph->data();
        //使用二分法快速查找所在点数据！！！敲黑板，下边这段是重点
        int low = 0, high = tmpContainer->size();
        while(high > low)
        {
            int middle = (low + high) / 2;
            if(x < tmpContainer->constBegin()->mainKey() ||
                    x > (tmpContainer->constEnd()-1)->mainKey())
                break;

            if(x == (tmpContainer->constBegin() + middle)->mainKey())
            {
                y = (tmpContainer->constBegin() + middle)->mainValue();
                break;
            }
            if(x > (tmpContainer->constBegin() + middle)->mainKey())
            {
                low = middle;
            }
            else if(x < (tmpContainer->constBegin() + middle)->mainKey())
            {
                high = middle;
            }
            if(high - low <= 1)
            {   //差值计算所在位置数据
                y = (tmpContainer->constBegin()+low)->mainValue() + ( (x - (tmpContainer->constBegin() + low)->mainKey()) *
                                                                      ((tmpContainer->constBegin()+high)->mainValue() - (tmpContainer->constBegin()+low)->mainValue()) ) /
                        ((tmpContainer->constBegin()+high)->mainKey() - (tmpContainer->constBegin()+low)->mainKey());
                break;
            }

        }

        if(info.vtrac)
        {
            info.vtrac->updateTracerPosition(x, y);
            info.vtrac->setText(QString::number(y, 'f', 2));
        }


    }
    m_plot->replot();

}

// 初始化参数
void MainWindow::InitParam()
{
    m_nXlength = 50;
    m_nYchanged = -1;

    //初始化默认颜色列表
    {
        m_listColorDef.push_back(QColor(250, 120, 0));
        m_listColorDef.push_back(QColor(0, 180, 60));
        m_listColorDef.push_back(Qt::green);
        m_listColorDef.push_back(Qt::yellow);
        m_listColorDef.push_back(Qt::black);
        m_listColorDef.push_back(Qt::blue);
        m_listColorDef.push_back(Qt::red);
        m_listColorDef.push_back(Qt::darkCyan);
    }

}
// 获取可用颜色
QColor MainWindow::GetUsefullColor(int i)
{
    if(i>= 0 && i < m_listColorDef.size())
        return m_listColorDef.at(i);
    else
    {
        //QColor c(222,222,222);
        QColor c(qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100);
        m_listColorDef.push_back(c);
        return c;
    }
}




void MainWindow::on_txtMainScaleNumY_3_returnPressed()
{
    VDial->setEndValue(ui->txtMainScaleNumY_3->text().toDouble());

}
