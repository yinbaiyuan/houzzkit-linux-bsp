#include "uarttest.h"
#include "QDialog"
#include "QLayout"
#include "QTextEdit"
#include "commoncomponent.h"
#include "QComboBox"
#include "QSerialPortInfo"
#include "QSerialPort"
#include "QDebug"
#include "QButtonGroup"
#include <QSettings>
#include <QTimer>

static CommonComponent componet;
static QTextEdit *reciveMessage;
static QTextEdit *sendMessage;
static QTimer *timer;

static QPushButton *serialBtn;
static QPushButton *sendBtn;
//QSerialPort *testSerial;
static QWidget *uartWidget;
static int i = 0;

UartTest::UartTest(QObject *parent) : QObject(parent){

//    if(testSerial!=nullptr){
//        testSerial->close();
//    }

    //    showWindow();
}

void UartTest::showWindow(int screenWidth, int screenHeight)
{
    i = 0;
    uartWidget = componet.getWidget();
    timer = new QTimer(uartWidget);

    QString ignoreTTY = componet.getConfig("serial", "ignore_tty");

    QHBoxLayout *boxLayout_h = new QHBoxLayout();
    QVBoxLayout *boxLayout_v = new QVBoxLayout(); //垂直
    QHBoxLayout *btnLayout_h = new QHBoxLayout();
    QGridLayout *uartLayout = new QGridLayout();

    QLabel *reciveLabel = componet.getLabel("接收端：");
    QLabel *sendLabel = componet.getLabel("发送端：");
    reciveMessage = componet.getTextEdit();
    reciveMessage->setDisabled(true);
    reciveMessage->setText("测试前请先将串口的tx和rx短接");

    sendMessage = componet.getTextEdit();
    sendMessage->setText("www.rpdzkj.com");

    QLabel *serial = componet.getLabel("串口:");
    QLabel *baudSel = componet.getLabel("波特率:");
    QLabel *dataBitSel = componet.getLabel("数据位:");
    QLabel *stopBitSel = componet.getLabel("停止位:");
    QLabel *flowContrlSel = componet.getLabel("流控:");

    //    QButtonGroup *serialGrp = new QButtonGroup();
    QGridLayout *serialLayout = new QGridLayout();


    QLineEdit *baudRate = componet.getLineEdit();
    baudRate->setText("115200");
    baudRate->setDisabled(true);

    QLineEdit *dataBit = componet.getLineEdit();
    dataBit->setText("8");
    dataBit->setDisabled(true);

    QLineEdit *stopBit = componet.getLineEdit();
    stopBit->setText("1");
    stopBit->setDisabled(true);

    QLineEdit *flowContrl = componet.getLineEdit();
    flowContrl->setText("0");
    flowContrl->setDisabled(true);

    QPushButton *closeWindow = componet.getButton("关闭窗口");
    connect(closeWindow, &QPushButton::clicked, [=](){this->getTestResult();});

    sendBtn = componet.getButton("发送数据");
    //    connect(sendBtn, &QPushButton::clicked, [=] { sendSerialData(sendMessage->toPlainText().toLatin1()); });

    QPushButton *cleanBtn = componet.getButton("清空数据");
    connect(cleanBtn, &QPushButton::clicked, reciveMessage, &QTextEdit::clear);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            if(serial.portName().indexOf("ttyUSB") < 0 && ignoreTTY.indexOf(serial.portName()) < 0)
            {
                i++;
                //uartList->addItem(serial.portName());
                //QRadioButton *serialBtn = new QRadioButton();

                serialBtn = componet.getButton(serial.portName());
                QSerialPort *testSerial = new QSerialPort(uartWidget);

                testSerial->setPortName(serial.portName());
                testSerial->open(QIODevice::ReadWrite);
                testSerial->setBaudRate(QSerialPort::Baud115200);
                testSerial->setDataBits(QSerialPort::Data8);
                testSerial->setParity(QSerialPort::NoParity);
                testSerial->setStopBits(QSerialPort::OneStop);
                testSerial->setFlowControl(QSerialPort::NoFlowControl);

                connect(testSerial, &QSerialPort::readyRead, [=] { reciveData(testSerial); });
                connect(sendBtn, &QPushButton::clicked, [=] { sendSerialData(sendMessage->toPlainText().toLatin1(),testSerial); });

                serialLayout->addWidget(serialBtn, i, 1);
                connect(serialBtn, &QPushButton::clicked, [=] { changeSerial(serialBtn->text()); });
                // serial.close();

                connect(timer, &QTimer::timeout, [=]  {this->sendSerialData(sendMessage->toPlainText().toLatin1(),testSerial);});
            }
        }
    }

    boxLayout_v->addWidget(reciveLabel);
    boxLayout_v->addWidget(reciveMessage);
    boxLayout_v->addWidget(sendLabel);
    boxLayout_v->addWidget(sendMessage);
    btnLayout_h->addWidget(sendBtn);
    btnLayout_h->addWidget(cleanBtn);
    boxLayout_v->addLayout(btnLayout_h);
    uartLayout->addWidget(serial, 1, 1, Qt::AlignRight);
    uartLayout->addLayout(serialLayout, 1, 2);
    uartLayout->addWidget(baudSel, 2, 1, Qt::AlignRight);
    uartLayout->addWidget(baudRate, 2, 2);
    uartLayout->addWidget(dataBitSel, 3, 1, Qt::AlignRight);
    uartLayout->addWidget(dataBit, 3, 2);
    uartLayout->addWidget(stopBitSel, 4, 1, Qt::AlignRight);
    uartLayout->addWidget(stopBit, 4, 2);
    uartLayout->addWidget(flowContrlSel, 5, 1, Qt::AlignRight);
    uartLayout->addWidget(flowContrl, 5, 2);
    // uartLayout->addWidget(openSerial,6,1);
    uartLayout->addWidget(closeWindow, 6, 2);
    timer->setInterval(1000);
    timer->start();
    boxLayout_h->addLayout(boxLayout_v);
    boxLayout_h->addLayout(uartLayout);
    uartWidget->setLayout(boxLayout_h);
    // uartWidget->move((screenWidth-uartWidget->width())/2, (screenHeight-uartWidget->height())/2);
    uartWidget->resize(screenWidth,screenHeight);
    uartWidget->showFullScreen();
}

void UartTest::sendSerialData(QString message, QSerialPort *testSerial)
{
    testSerial->open(QIODevice::ReadWrite);

    if( testSerial != nullptr && testSerial->isOpen())

        testSerial->write(message.toLocal8Bit());

    else
        reciveMessage->append("请先选择串口！");

}

void UartTest::changeSerial(QString serialportName)
{
    reciveMessage->clear();

//    if(testSerial!=nullptr){
//        testSerial->close();
//    }

    if(serialportName != ""){
        //初始化串口
        //        serial = new QSerialPort;
        //        serial->setPortName(serialportName);
        //        serial->open(QIODevice::ReadWrite);
        //        serial->setBaudRate(QSerialPort::Baud115200);
        //        serial->setDataBits(QSerialPort::Data8);
        //        serial->setParity(QSerialPort::NoParity);
        //        serial->setStopBits(QSerialPort::OneStop);
        //        serial->setFlowControl(QSerialPort::NoFlowControl);
        //        connect(serial, &QSerialPort::readyRead, [=] { reciveData(); });
    }
}

void UartTest::reciveData(QSerialPort *testSerial)
{
    QByteArray buf;
    buf = testSerial->readAll();
    if(!buf.isEmpty()){
        QString dis = buf;
        reciveMessage->append(testSerial->portName() + ":" + dis);

        QList<QPushButton *> buttons = uartWidget->findChildren<QPushButton *>();
        for (QPushButton *btn : buttons) {
            if(btn->text() == testSerial->portName()){
                btn->setStyleSheet(btn->styleSheet()+"QPushButton{border-image:url(:/icon/pass.png)}");
            }
             btn->update();
        }
    }

    buf.clear();
}

void UartTest::getTestResult()
{
    QList<QPushButton *> buttons = uartWidget->findChildren<QPushButton *>();
    int successCount = 0;
    for (QPushButton *btn : buttons) {
        if(btn->text().startsWith("tty")){
//            btn->setStyleSheet(btn->styleSheet()+"QPushButton{border-image:url(:/icon/pass.png)}");
            if(btn->styleSheet().indexOf("pass") >= 0)
                successCount ++;
        }
    }

    //qDebug()<<"successCount:"<<successCount;
    //qDebug()<<"i============:"<<i;

    if(successCount == i)
        result = true;
    else
        result = false;

    emit update_btn_style();
    uartWidget->close();
}





