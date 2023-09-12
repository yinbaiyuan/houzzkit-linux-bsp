#include "bluetooth.h"
#include "commoncomponent.h"
#include "QListWidget"
#include "QProcess"
#include "QDebug"
#include "stdlib.h"
#include "QFile"
#include <QTimer>

static CommonComponent componet;

static QString initBTCmd1 = "/usr/bin/bt_pcba_test";
static QString initBTCmd2 = "/usr/bin/bt_init.sh";
static QString openBTCmd = "hciconfig hci0 up";
static QString btport = "/sys/class/bluetooth/hci0";

static QString scanBTCmd = "hcitool lescan";
static QTimer *timer1;
static QTimer *timer2;
static QTimer *timer3;

static QWidget *widget;
static QPushButton *tips;
static int testCount = 0;

BlueTooth::BlueTooth(QObject *parent) : QObject(parent)
{
    //    initWindow();
}

void BlueTooth::initWindow(int screenWidth, int screenHeight)
{
    testCount = 0;
    widget = componet.getWidget();
    timer1 = new QTimer(widget);
    timer2 = new QTimer(widget);
    timer3 = new QTimer(widget);

    connect(timer1, &QTimer::timeout, [=] { openBT(); });

    QGridLayout *layout = new QGridLayout();

    tips = componet.getButton("正在自动测试蓝牙功能，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    layout->addWidget(tips, 1, 1, 1, 4);

    widget->setLayout(layout);
    //    widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);
    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();
    timer1->start(100);
}

void BlueTooth::openBT()
{
    timer1->stop();
    QFile bashFile(initBTCmd1);

    if(bashFile.exists())
        ::system(initBTCmd1.toLocal8Bit());
    else
        ::system(initBTCmd2.toLocal8Bit());

    connect(timer2, &QTimer::timeout, [=] { getBTdev(); });
    timer2->setInterval(500);
    timer2->start();
}

void BlueTooth::getBTdev()
{
    ::system("hciconfig -a > /userdata/hciconfig.txt");
    QFile file("/userdata/hciconfig.txt");
    if(file.open(QIODevice::ReadWrite)){
        QString tesTresult = file.readAll();
        if(tesTresult.indexOf("hci0") >= 0){
            timer2->stop();
            result = true;
            file.close();
            tips->setText("正在自动测试蓝牙功能，测试成功！！");
            getTestResult();
        }else{
            testCount ++ ;
            if(testCount == 4){
                timer2->stop();
                result = false;
                tips->setText("正在自动测试蓝牙功能，测试失败！！");
                getTestResult();
            }
        }
    }else{
        timer2->stop();
        result = false;
        tips->setText("正在自动测试蓝牙功能，测试失败！！");
        getTestResult();
    }
}

void BlueTooth::getTestResult()
{
    connect(timer3, &QTimer::timeout, [=] {widget->close();});
    emit update_btn_style();
    timer3->start(1000);

}
