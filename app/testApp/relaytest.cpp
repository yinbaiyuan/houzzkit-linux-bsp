#include "relaytest.h"
#include "commoncomponent.h"
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QDir>

static CommonComponent componet;
static QLabel *tips;
static QTimer *timer;
static QWidget *widget;
RelayTest::RelayTest(QObject *parent) : QObject(parent)
{

}

void RelayTest::initWindow(int screenWidth, int screenHeight)
{
    widget = componet.getWidget();
    QGridLayout *layout = new QGridLayout;
    tips = componet.getLabel("自动拉高拉低继电器电压，请注意听是否有响声！");
    QPushButton *passBtn = componet.getButton("测试成功");
    QPushButton *faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");

    connect(passBtn, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faildBtn, &QPushButton::clicked, [=] { this->getTestResult(false); });

    layout->addWidget(tips, 1, 1, 1, 2, Qt::AlignCenter);
    layout->addWidget(passBtn, 2, 1, 1, 1);
    layout->addWidget(faildBtn, 2, 2, 1, 1);

    QString relayRpGpio = componet.getConfig("relay", "rp_gpio");

    timer = new QTimer(widget);
    connect(timer, &QTimer::timeout, [=] {this->handleTimeout(relayRpGpio);});
    timer->start(1000);

    widget->resize(screenWidth,screenHeight);
    widget->setLayout(layout);
    widget->showFullScreen();
}

void RelayTest::getTestResult(bool tesTresult)
{
    timer->stop();
    result=tesTresult;
    emit update_btn_style();
}

QString relayStatus = "1";
void RelayTest::handleTimeout(QString rp_gpio)
{
    timer->stop();
    if(!componet.checkConfig(rp_gpio)){
        result = true;
        tips->setText(tips->text()+"当前版型无继电器!");
        disconnect(timer,0 ,0 ,0);
        connect(timer, &QTimer::timeout, [=] {widget->close();});
        timer->start(1000);
        emit update_btn_style();

    }else{
        connect(timer, &QTimer::timeout, [=] {this->pullRelay(relayStatus);});
        timer->setInterval(1000);
        timer->start();
    }

}

void RelayTest::pullRelay(QString status)
{
    if(relayStatus == "1"){
        relayStatus = "0";
        ::system(QString("echo 1 > /proc/rp_gpio/%1").arg(status).toLocal8Bit());
        qDebug()<<"pull up!";
        return;
    }else{
        relayStatus = "1";
        ::system(QString("echo 0 > /proc/rp_gpio/%1").arg(status).toLocal8Bit());
        qDebug()<<"pull down!";
        return;
    }

}

