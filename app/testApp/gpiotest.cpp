#include "gpiotest.h"
#include "commoncomponent.h"
#include <stdlib.h>
#include <QDebug>
#include <QFile>

static CommonComponent componet;
static QLineEdit *gpioNum;
static QLineEdit *gpioValue;
static QComboBox *directionBox;

GpioTest::GpioTest(QObject *parent) : QObject(parent)
{

}

void GpioTest::initWindow(int screenWidth, int screenHeight)
{
    QWidget *widget = componet.getWidget();
    QGridLayout *layout = new QGridLayout();
    QLabel *gpioNumLabel = componet.getLabel("GPIO:");
    gpioNum = componet.getLineEdit();
    gpioNum->setText("GPIO3_C2");
    QLabel *gpioValLabel = componet.getLabel("Value:");
    gpioValue = componet.getLineEdit();
    QLabel *directionLabel = componet.getLabel("方向");
    directionBox = componet.getComboBox();
    directionBox->addItem("out");
    directionBox->addItem("in");


    QPushButton *pullUp = componet.getButton("拉高");
    QPushButton *pullDown = componet.getButton("拉低");
    QPushButton *readValue = componet.getButton("读取");
    connect(pullUp, &QPushButton::clicked, [=] { this->setValToHigh(1); });
    connect(pullDown, &QPushButton::clicked, [=] { this->setValToLow(0); });
    connect(readValue, &QPushButton::clicked, [=] { this->readValue(); });

    QHBoxLayout *titleBar = componet.getTitleBar(widget, "GPIO功能测试");

    layout->addLayout(titleBar, 1, 1, 1, 4);
    layout->addWidget(gpioNumLabel, 2, 1);
    layout->addWidget(gpioNum, 2, 2, 1, 2);
    layout->addWidget(directionLabel, 3, 1);
    layout->addWidget(directionBox, 3, 2, 1, 2);
    layout->addWidget(pullUp, 4, 1);
    layout->addWidget(pullDown, 4, 2);
    layout->addWidget(readValue, 4, 3);
    layout->addWidget(gpioValLabel, 5, 1);
    layout->addWidget(gpioValue, 5, 2, 1, 2);

    widget->setLayout(layout);
    widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);
    widget->show();
}

void GpioTest::setValToLow(int value)
{
    if(convertGpio(gpioNum->text()) == -1){
        gpioValue->setText("this GPIO is not export");
        return;
    }else{
        QString exportGpio = QString("echo %1 > /sys/class/gpio/export").arg(convertGpio(gpioNum->text()));
        QString setDirection = QString("echo %1 > /sys/class/gpio/gpio%2/direction").arg(directionBox->currentText()).arg(convertGpio(gpioNum->text()));
        QString setValue = QString("echo %1 > /sys/class/gpio/gpio%2/value").arg(value).arg(convertGpio(gpioNum->text()));

//        qDebug()<<exportGpio;
//        qDebug()<<setDirection;
//        qDebug()<<setValue;

        //指令带有重定向 > 符号，只能使用system函数，调用系统函数使用 ::
        ::system(exportGpio.toLocal8Bit());
        ::system(setDirection.toLocal8Bit());
        ::system(setValue.toLocal8Bit());

        readValue();
    }
}

void GpioTest::setValToHigh(int value)
{
    setValToLow(value);
}

void GpioTest::readValue()
{
    if(convertGpio(gpioNum->text()) == -1){
        gpioValue->setText("this GPIO is not export");
        return;
    }
    QString path = "/sys/class/gpio/gpio" + QString::number(convertGpio(gpioNum->text())) + "/value";
    qDebug()<<path;
    QFile gpioValueFile(path);

    if(!gpioValueFile.open(QIODevice::ReadOnly)){
        gpioValue->setText("this GPIO is not export");
    }else{
        QString value = gpioValueFile.readAll();
        if(value == "1\n"){
            gpioValue->setText("1");
        }else{
            gpioValue->setText("0");
        }
    }

}

int GpioTest::convertGpio(QString gpioStr)
{
//    qDebug()<<gpioStr;

    if(gpioStr != "" && gpioStr.indexOf("_") != -1){
        QStringList list = gpioStr.replace("GPIO","").replace("_","").split("");//3C2
        if(list.at(2) == "A"){
            return list.at(1).toInt() * 32 + (1-1)*8 + list.at(3).toInt();

        }else if(list.at(2) == "B"){
            return list.at(1).toInt() * 32 + (2-1)*8 + list.at(3).toInt();

        }else if(list.at(2) == "C"){
            return list.at(1).toInt() * 32 + (3-1)*8 + list.at(3).toInt();

        }else if(list.at(2) == "D"){
            return list.at(1).toInt() * 32 + (4-1)*8 + list.at(3).toInt();

        }else if(list.at(2) == "E"){
            return list.at(1).toInt() * 32 + (5-1)*8 + list.at(3).toInt();

        }else{
            return -1;//invalid
        }
    }else{
        return -1;
    }
}
