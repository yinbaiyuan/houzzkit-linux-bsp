#include "usbtest.h"
#include "commoncomponent.h"

static CommonComponent componet;

static QString devPath = "/dev/sda";

USBTest::USBTest(QObject *parent) : QObject(parent)
{

}

void USBTest::initWindow(int screenWidht, int screenHeight)
{
    widget = componet.getWidget();
    timer = new QTimer(widget);

    tips = componet.getButton("正在自动测试USB，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(tips);
    widget->setLayout(layout);
//  widget->move((winWidth-widget->width())/2, (winHeight-widget->height())/2);

    connect(timer, &QTimer::timeout, [=] {getTestResult();});
    widget->resize(screenWidht,screenHeight);
    widget->showFullScreen();
    timer->start(1000);
}


void USBTest::getTestResult()
{
    timer->stop();
    QFile usbDev(devPath);

    if(usbDev.exists()){
        tips->setText("正在自动测试USB，测试成功！！！");
        result = true;
    }else{
        tips->setText("正在自动测试USB，测试失败！！！");
        result = false;
    }
    disconnect(timer,0 ,0 , 0);
    connect(timer, &QTimer::timeout, [=] {widget->close();});
    timer->start(1000);
    emit update_btn_style();
}
