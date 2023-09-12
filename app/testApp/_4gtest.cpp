#include "_4gtest.h"
#include "commoncomponent.h"
#include "QFile"
#include "QProcess"
#include "QDebug"
#include "QElapsedTimer"
#include "mainwindow.h"
#include <QTimer>

static CommonComponent componet;
static QString devPort = "/dev/ttyUSB3";
static QString defaultTestIp = "www.rpdzkj.com";
static QLineEdit *ipEdit;
static QTextEdit *testResult;
static QProcess process;
static QString callCmd = "pppd call quectel-ppp";
static QWidget *widget;
static QPushButton *callBtn;
static QPushButton *tips;

static QTimer *timer1;
static QTimer *timer2;
static QTimer *timer3;
static QTimer *timer4;
static int readCount = 0;

_4GTest::_4GTest(QObject *parent) : QObject(parent)
{
    //    initWindow();
}

void _4GTest::initWindow(int screenWidth, int screenHeight)
{
    readCount = 0;
    widget = componet.getWidget();
    timer1 = new QTimer(widget);
    timer2 = new QTimer(widget);
    timer3 = new QTimer(widget);
    timer4 = new QTimer(widget);

    QGridLayout *layout = new QGridLayout();

    QFile devFile(devPort);
    //    QHBoxLayout *titleBar = componet.getTitleBar(widget,"4G模块通信测试");
    //    layout->addLayout(titleBar, 1, 1, 1, 5);
    tips = componet.getButton("正在自动测试4G拨号，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");
    layout->addWidget(tips, 1, 1, 1, 5);

    if(!devFile.exists()){
        //没有找到ttyUSB节点，4G模块通信失败
        QLabel *notFoundTtyusb = componet.getLabel("没有找到ttyUSB节点，模块通信失败！");
        layout->addWidget(notFoundTtyusb, 2, 1, 5, 5, Qt::AlignCenter);
        result=false;
        qDebug()<<"没有找到ttyUSB节点，模块通信失败！";
        getTestResult();

    }else{
        //成功识别到ttyUSB节点, 然后拨号
        QLabel *ipLabel = componet.getLabel("4Gip:");
        ipEdit = componet.getLineEdit();
        ipEdit->setDisabled(true);
        ipEdit->setText("IP not found");
        callBtn = componet.getButton("拨号");

        QLabel *testIpLabel = componet.getLabel("测试网址:");
        QLineEdit *testIpEdit = componet.getLineEdit();
        testIpEdit->setText(defaultTestIp);
        QPushButton *testBtn = componet.getButton("ping");

        connect(callBtn, &QPushButton::clicked, [=] { call(); });
        connect(testBtn, &QPushButton::clicked, [=] { test(testIpEdit->text()); });

        QLabel *resultLabel = componet.getLabel("测试结果:");
        testResult = componet.getTextEdit();

        layout->addWidget(ipLabel, 2, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);
        layout->addWidget(ipEdit, 2, 2, 1, 3);
        layout->addWidget(callBtn, 2, 5, 1, 1);

        layout->addWidget(testIpLabel, 3, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);
        layout->addWidget(testIpEdit, 3, 2, 1, 3);
        layout->addWidget(testBtn, 3, 5, 1, 1);

        layout->addWidget(resultLabel, 4, 1, 1, 5);
        layout->addWidget(testResult, 5, 1, 5, 5);
        connect(timer1, &QTimer::timeout, [=] { delyCall(); });
        timer1->start(100);
    }
    widget->setLayout(layout);
    // widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);
    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();

}

void _4GTest::test(QString targetAddr)
{
    testResult->clear();
    if(ipEdit->text() == "IP not found" || ipEdit->text() == ""){
        testResult->append("获取IP失败，请先拨号！");
        return;
    }

    QString testCmd = QString("ping -S %1 -s 1 -c 1 %2").arg(ipEdit->text()).arg(targetAddr);
    process.start(testCmd);
    process.waitForReadyRead(1000);
    process.waitForFinished(1000);
    //    process.close();
    QString sRet = process.readAll();
    if(sRet.indexOf("ttl") >= 0){
        testResult->append(sRet.toUtf8());
        testResult->append("测试成功！");
    }else{
        testResult->append(sRet.toUtf8());
        testResult->append("测试失败！");
    }

}

void _4GTest::delyCall()
{
    timer1->stop();
    callBtn->setText("拨号中");
    connect(timer2, &QTimer::timeout, [=] { call(); });
    timer2->start(200);
}

void _4GTest::call()
{
    timer2->stop();
    //    ::system("ifconfig eth0 down && ifconfig lo down && ifconfig wlan0 down");

    ::system("sh /etc/ppp/peers/quectel-pppd.sh > /userdata/4Gcall_result");
    connect(timer3, &QTimer::timeout, [=] {readMessage();});

    timer3->setInterval(200);
    // timer3->start(2000);
    timer3->start();
}

void _4GTest::readMessage(){
//    timer3->stop();
    QString sRet;
    QFile call_result("/userdata/4Gcall_result");
    if(!call_result.open(QIODevice::ReadWrite)){
        testResult->append("拨号失败");
        tips->setText("正在自动测试4G拨号，拨号失败！！");
        result = false;
        getTestResult();
        return;
    }

    sRet = call_result.readAll();
    testResult->clear();
    testResult->append(sRet);

    //qDebug()<<sRet;

    if(sRet.indexOf("local  IP address") >= 0){
        timer3->stop();
        qDebug()<<"successful!!!";
        QList<QString> resultList = sRet.split("\n");
        for(QString str : resultList){
            if(str.startsWith("local  IP address")){
                QString ipAddress = str.replace("local  IP address", "").replace(" ","");
                ipEdit->setText(ipAddress);
                testResult->append("\n拨号成功！！");
                tips->setText("正在自动测试4G拨号，拨号成功！！");
                result = true;
                getTestResult();
                break;
            }
        }
    }else if (sRet.indexOf("is locked by pid") >= 0){
        timer3->stop();
        testResult->append("\n已拨号，请勿重复拨号！！");
        result = true;
        getTestResult();
    }else{
        readCount ++;
        if(readCount >= 25){
            timer3->stop();
            qDebug()<<"faild!!!";
            testResult->append("\n拨号失败！！");
            tips->setText("正在自动测试4G拨号，拨号失败！！");
            result = false;
            getTestResult();
        }
    }

    call_result.close();
    //call_result.remove();
}

void _4GTest::getTestResult()
{
    connect(timer4, &QTimer::timeout, [=] { widget->close(); });
    timer4->start(1000);
    emit update_btn_style();
}

