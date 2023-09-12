#include "otg_host.h"
#include <QTimer>
#include "commoncomponent.h"
#include <QDebug>

static CommonComponent componet;
static QLabel *label;
static QTimer *timer;
static QWidget *widget;

Otg_Host::Otg_Host(QObject *parent) : QObject(parent)
{

}

void Otg_Host::initWindow(int winWidth, int winHeight)
{
    widget = componet.getWidget();
    QGridLayout *layou = new QGridLayout();
    label = componet.getLabel("");
    label->setAlignment(Qt::AlignCenter);

    rp_gpio = componet.getConfig("otg_host", "rp_gpio");
    otg_dev_str = componet.getConfig("otg_host", "otg_dev");
    host_dev_str = componet.getConfig("otg_host", "host_dev");

    QFile otg_dev(otg_dev_str);
    QFile host_dev(host_dev_str);

    qDebug()<<"get config rp_gpio:"<<rp_gpio;
    qDebug()<<"get config otg_dev:"<<otg_dev_str;
    qDebug()<<"get config host_dev:"<<host_dev_str;

    if((!componet.checkConfig(rp_gpio)) && (!otg_dev.exists() && !host_dev.exists())){

        timer = new QTimer(widget);
        layou->addWidget(label, 1, 1, 1, 1);
        label->setText("当前系统版型无法切换OTG_HOST模式！");
        connect(timer, &QTimer::timeout, [=] {this->handleTimeout(false);});
        timer->start(1000);
    }else{
        QPushButton *otgBtn = componet.getButton("OTG模式");
        QPushButton *hostBtn = componet.getButton("HOST模式");
        QPushButton *closeBtn = componet.getButton("关闭窗口");

        connect(hostBtn, &QPushButton::clicked, [=] { changMode("HOST", rp_gpio); });
        connect(otgBtn, &QPushButton::clicked, [=] { changMode("OTG", rp_gpio); });
        connect(closeBtn, &QPushButton::clicked, [=] { widget->close(); });

        layou->addWidget(otgBtn, 1, 1, 1, 2);
        layou->addWidget(hostBtn,1, 3, 1, 2);
        layou->addWidget(label, 2, 1, 1, 4);
        layou->addWidget(closeBtn, 3, 2, 1, 2);
    }

    widget->setLayout(layou);
    widget->resize(winWidth,winHeight);
//    widget->move((winWidth-widget->width())/2, (winHeight-widget->height())/2);
    widget->showFullScreen();
}

void Otg_Host::changMode(QString mode, QString rp_gpio)
{
    label->setText("");
    if(mode == "HOST"){
        label->setText("切换至HOST模式！");
        if (componet.checkConfig(rp_gpio))
            ::system(QString("echo 1 > /proc/rp_gpio/%1").arg(rp_gpio).toLocal8Bit());
        else
            ::system(QString("cat %1").arg(host_dev_str).toLocal8Bit());

    }else{
        label->setText("切换至OTG模式！");
        if (componet.checkConfig(rp_gpio))
            ::system(QString("echo 0 > /proc/rp_gpio/%1").arg(rp_gpio).toLocal8Bit());
        else
            ::system(QString("cat %1").arg(otg_dev_str).toLocal8Bit());
    }
}

void Otg_Host::handleTimeout(bool testResult)
{
    result = testResult;
    widget->close();
    emit update_btn_style();
}
