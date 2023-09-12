#include "rtctest.h"
#include "QDateTimeEdit"
#include "commoncomponent.h"
#include "QDebug"
#include "time.h"
#include "QProcess"
#include <QTimer>

static CommonComponent componet;
static QTimer *timer;
static QString rtcDevPath = "/sys/class/rtc/rtc0";
static QWidget *widget;
static QPushButton *tips;

RtcTest::RtcTest(QObject *parent) : QObject(parent)
{

}

void RtcTest::inintWindow(int winWidth, int winHeight)
{
    widget = componet.getWidget();
    timer = new QTimer(widget);

    tips = componet.getButton("正在自动测试RTC，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(tips);
    widget->setLayout(layout);
//  widget->move((winWidth-widget->width())/2, (winHeight-widget->height())/2);

    connect(timer, &QTimer::timeout, [=] {getTestResult();});
    widget->resize(winWidth,winHeight);
    widget->showFullScreen();
    timer->start(1000);

}

void RtcTest::getTestResult()
{
    timer->stop();
    QFile file(rtcDevPath);
    if(file.exists()){
        tips->setText("正在自动测试RTC，测试成功！！！");
        result = true;
    }else{
         tips->setText("正在自动测试RTC，测试失败！！！");
        result = false;
    }
    connect(timer, &QTimer::timeout, [=] {widget->close();});
    timer->start(1000);
    emit update_btn_style();
}


