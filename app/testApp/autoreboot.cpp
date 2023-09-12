#include "autoreboot.h"
#include "commoncomponent.h"
#include "QFile"
#include "QDebug"
#include "QTimer"
#include "QProcess"

QProcess process;

static CommonComponent componet;

AutoReboot::AutoReboot(QObject *parent) : QObject(parent)
{
    initWindow();
}

void AutoReboot::initWindow()
{
    QFile rebootFlag("./rebootFlag");
    if(!rebootFlag.open(QIODevice::ReadWrite)){
        qDebug()<<"Failed to read file!";
        return;
    }
    QString flag = rebootFlag.readLine();

    if(flag == "1"){
        QWidget *widget = componet.getWidget();
        QPushButton *stopRebootBtn = componet.getButton("停止重启");
        stopRebootBtn->setFixedSize(200,100);
        connect(stopRebootBtn, &QPushButton::clicked, [=] { stopReboot(); });
        QVBoxLayout *layout = new QVBoxLayout();

        layout->addLayout(componet.getTitleBar(widget,"重启测试"));
        layout->addWidget(stopRebootBtn);
        layout->setAlignment(stopRebootBtn,Qt::AlignVCenter|Qt::AlignHCenter);
        widget->setLayout(layout);
        widget->show();
        widget->update();

//        QTimer *timer = new QTimer(this);
//        timer->setInterval(3000);
//        connect(timer, &QTimer::timeout, [=] { startReboot(); });

//        timer->start();
//        rebootFlag.close();
    }
}

void AutoReboot::stopReboot()
{
    QFile rebootFlag("./rebootFlag");
    if(!rebootFlag.open(QIODevice::ReadWrite)){
        qDebug()<<"Failed to read file!";
        return;
    }
    rebootFlag.write(QString("0").toLocal8Bit());
    rebootFlag.close();
}

void AutoReboot::startReboot()
{
    QFile rebootFlag("./rebootFlag");
    if(!rebootFlag.open(QIODevice::ReadWrite)){
        qDebug()<<"Failed to read file!";
        return;
    }

    QString flag = rebootFlag.readLine();
    if(flag == "1"){
        rebootFlag.close();
        process.start("reboot -f");
    }
}
