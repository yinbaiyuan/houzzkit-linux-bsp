#include "ledtest.h"
#include "commoncomponent.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QSettings>

static CommonComponent componet;
static QTimer *timer;
static QWidget *widget;
static QString bl_rp_gpio;
static QString ir_rp_gpio;
static int ledStatus = 0;
static int timeCount = 0;

LedTest::LedTest(QObject *parent) : QObject(parent)
{

}

void LedTest::initWindow(int winWidth, int winHeight)
{   
    ledStatus = 0;
    timeCount = 0;

    bl_rp_gpio = componet.getConfig("led","bl_rp_gpio");
    ir_rp_gpio = componet.getConfig("led","ir_rp_gpio");

    widget = componet.getWidget();
    QGridLayout *layout = new QGridLayout();
    QLabel *irLed = componet.getLabel("红外灯：");
    QLabel *blLed = componet.getLabel("补光灯：");
    QPushButton *tips = componet.getButton("自动拉高拉低补光灯和红外灯，请注意观察补光灯和红外灯是否有闪烁！");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QPushButton *pullUpIr = componet.getButton("打开");
    QPushButton *pullDownIr = componet.getButton("关闭");
    QPushButton *pullUpBl = componet.getButton("打开");
    QPushButton *pullDownBl= componet.getButton("关闭");
    QPushButton *passBtn = componet.getButton("测试成功");
    QPushButton *faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");

    connect(pullUpIr, &QPushButton::clicked, [=] { this->setValue(ir_rp_gpio,1); });
    connect(pullDownIr, &QPushButton::clicked, [=] { this->setValue(ir_rp_gpio,0); });
    connect(pullUpBl, &QPushButton::clicked, [=] { this->setValue(bl_rp_gpio,1); });
    connect(pullDownBl, &QPushButton::clicked, [=] { this->setValue(bl_rp_gpio,0); });

    connect(passBtn, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faildBtn, &QPushButton::clicked, [=] { this->getTestResult(false); });

    layout->addWidget(tips, 1, 1, 1, 3);
    layout->addWidget(irLed, 2, 1, 1, 1);
    layout->addWidget(pullUpIr, 2, 2, 1, 1);
    layout->addWidget(pullDownIr, 2, 3, 1, 1);
    layout->addWidget(blLed, 3, 1, 1, 1);
    layout->addWidget(pullUpBl, 3, 2, 1, 1);
    layout->addWidget(pullDownBl, 3, 3, 1, 1);

    layout->addWidget(passBtn, 4, 1, 1, 1);
    layout->addWidget(faildBtn, 4, 3, 1, 1);

    widget->setLayout(layout);

    timer = new QTimer(widget);
    connect(timer, &QTimer::timeout, [=]  {this->handleTimeout();});
    timer->setInterval(1000);
    timer->start();

//    widget->resize(400,400);
//    widget->move((winWidth-widget->width())/2, (winHeight-widget->height())/2);
    widget->resize(winWidth,winWidth);
    widget->showFullScreen();

}

void LedTest::setValue(QString ledname, int value)
{
    QString cmd = QString("echo %1 > /proc/rp_gpio/%2").arg(value).arg(ledname);
    qDebug()<<cmd;
    ::system(cmd.toLocal8Bit());
}

void LedTest::handleTimeout()
{
    qDebug()<<"handleTimeout";
    timeCount++;
    if(timeCount >= 4){
        timer->stop();
        timeCount = 0;
        setValue(bl_rp_gpio,0);
        setValue(ir_rp_gpio,0);
    }else{
        ledStatus == 0 ? ledStatus = 1 : ledStatus = 0;

        if(componet.checkConfig(bl_rp_gpio))
            setValue(bl_rp_gpio,ledStatus);

        if(componet.checkConfig(ir_rp_gpio))
            setValue(ir_rp_gpio,ledStatus);
    }
}

void LedTest::getTestResult(bool testResult)
{
    result = testResult;
    emit update_btn_style();
    widget->close();
}
