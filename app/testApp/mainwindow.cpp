#include "mainwindow.h"
#include "QGuiApplication"
#include "QScreen"
#include "QLayout"
#include "QLabel"
#include "QDebug"
#include "QSignalMapper"
#include "uarttest.h"
#include "QFile"
#include "QProcess"
#include "QTimer"
#include "QDialog"
#include "QFileDialog"
#include <QScrollArea>
#include <QSettings>

#include "commoncomponent.h"
#include "arecord.h"
#include "ethnettest.h"
#include "gpiotest.h"
#include "autoreboot.h"
#include "wifitest.h"
#include "bluetooth.h"
#include "_4gtest.h"
#include "pwmtest.h"
#include "rtctest.h"
#include "ledtest.h"
#include "otg_host.h"
#include "relaytest.h"
#include "tfcardtest.h"
#include "usbtest.h"
#include "rgbtest.h"
#include "memtest.h"
#include "cameratest.h"

QRect windSize;
const QString funLists[] = {"以太网", "wifi", "蓝牙", "4G",
                            "音频", "串口", "TF卡", "otg/host",
                            "GPIO", "RTC", "PWM", "补光灯", "继电器",
                            "重启测试", "恢复出厂设置","自动测试", "USB",
                            "RGB","DDR和EMMC","USB摄像头"};

CommonComponent componet;
static QProcess process;
static QTimer *timer;
static int autoTestFlag = 0;

//QString configFilePath = "P:/lixin/rv1126_rv1109_new/debug/rv1126_rv1109/app/testApp/config.ini";
//QString configFilePath = "./config.ini";
//#define CONFIG_FILE_PATH "./config.ini"

MainWindow::MainWindow()
{
    this->initWindow();

    //这里可能有bug  如果在构造函数里面调用子类的show()方法，会出现窗口无法显示的情况,windows端不影响
    //    QFile rebootFlag("./rebootFlag");
    //    if(!rebootFlag.open(QIODevice::ReadWrite)){
    //        qDebug()<<"Failed to read file!";
    //        return;
    //    }
    //    QString flag = rebootFlag.readLine();
    //    if(flag == "1"){
    //        qDebug()<<"readflag is :"<<flag;
    //        AutoReboot *autoReboot = new AutoReboot;
    //        autoReboot->initWindow();
    //    }

}

void MainWindow::initWindow()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    windSize = screen->virtualGeometry();
    resize(windSize.width(), windSize.height());

    QGridLayout *mainLayout = new QGridLayout;
    QWidget *widget = new QWidget();
    widget->setAttribute(Qt::WA_DeleteOnClose);

    QSettings  *setting = new QSettings(CONFIG_FILE_PATH,QSettings::IniFormat);
    QStringList groups = setting->childGroups();

    int m = 1;
    int n = 0;
    for(int i = 0;i < groups.length(); i++){
        QString enable = setting->value("/"+groups.at(i)+"/enable").toString();
        int id = setting->value("/"+groups.at(i)+"/id").toInt();
        /* qDebug()<<groups.at(i)<<enable;
        qDebug()<<getCNName(id-1)<<id; */

        if(enable == "1"){
            QPushButton *testBtn = componet.getButton(getCNName(id-1));
            testBtn->setStyleSheet(testBtn->styleSheet()+"QPushButton{width:200;height:60}");
            connect(testBtn, &QPushButton::clicked, [=] { testClicked(testBtn->text()); });

            if(n % 2 == 0){
                mainLayout->addWidget(testBtn, m, 1, Qt::AlignRight);

            }else{
                mainLayout->addWidget(testBtn, m, 2, Qt::AlignLeft);
                m++;
            }

            n++;
        }
    }
    mainLayout->setHorizontalSpacing(60);
    widget->resize(windSize.width(), windSize.height());
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);
}

void MainWindow::testClicked(QString str)
{
    if(autoTestFlag){
        timer->stop();
    }
    // 根据传进来的参数对功能逐个测试
    if(str == "串口"){
        //qDebug()<<str;
        UartTest *uartTest = new UartTest(this);
        connect(uartTest, &UartTest::update_btn_style, [=] { testResult(str,uartTest->result); });
        uartTest->showWindow(windSize.width(),windSize.height());

    }else if(str == "wifi"){
        WifiTest *wifiTest = new WifiTest(this);
        connect(wifiTest, &WifiTest::update_btn_style, [=] { testResult(str,wifiTest->result); });
        wifiTest->initWindow(windSize.width(),windSize.height());
        //qDebug()<<str;

    }else if(str == "蓝牙"){
        BlueTooth *bluetooth = new BlueTooth(this);
        connect(bluetooth, &BlueTooth::update_btn_style, [=] { testResult(str,bluetooth->result); });
        bluetooth->initWindow(windSize.width(),windSize.height());
        //qDebug()<<str;

    }else if(str == "4G"){
        _4GTest *test4G = new _4GTest;
        connect(test4G, &_4GTest::update_btn_style, [=] { testResult(str,test4G->result); });
        test4G->initWindow(windSize.width(),windSize.height());
        //delete test4G;

    }else if(str == "音频"){
        ARecord *arecord = new ARecord(this);
        connect(arecord, &ARecord::update_btn_style, [=] { testResult(str,arecord->result); });
        arecord->initWindow(windSize.width(),windSize.height());

    }else if(str == "USB"){
        USBTest *usbTest = new USBTest(this);
        connect(usbTest, &USBTest::update_btn_style, [=] { testResult(str,usbTest->result); });
        usbTest->initWindow(windSize.width(),windSize.height());
        //qDebug()<<str;

    }else if(str == "TF卡"){
        TFcardTest *tfCardTest = new TFcardTest(this);
        connect(tfCardTest, &TFcardTest::update_btn_style, [=] { testResult(str,tfCardTest->result); });
        tfCardTest->initWindow(windSize.width(),windSize.height());

    }else if(str == "以太网"){
        EthnetTest *ethtest = new EthnetTest(this);
        connect(ethtest, &EthnetTest::update_btn_style, [=] { testResult(str,ethtest->result); });
        ethtest->initWindow(windSize.width(),windSize.height());

    }else if(str == "恢复出厂设置"){
        process.start("update");

    }else if(str == "重启测试"){
        //        QFile rebootFlag("./rebootFlag");
        //        if(!rebootFlag.open(QIODevice::ReadWrite)){
        //            qDebug()<<"Failed to cread file!";
        //            return;
        //        }
        //        rebootFlag.write(QString("1").toLocal8Bit());
        //        rebootFlag.close();
        //        AutoReboot *autoReboot = new AutoReboot;
        process.start("reboot -f");

        //        qDebug()<<str;

    }else if(str == "压力测试"){
        qDebug()<<str;

    }else if(str == "otg/host"){
        Otg_Host *changeMode = new Otg_Host(this);
        connect(changeMode, &Otg_Host::update_btn_style, [=] { testResult(str,changeMode->result); });
        changeMode->initWindow(windSize.width(),windSize.height());
        qDebug()<<str;

    }else if(str == "GPIO"){
        GpioTest gpiotest;
        gpiotest.initWindow(windSize.width(),windSize.height());
        qDebug()<<str;

    } else if(str == "RTC"){
        RtcTest *rtcTest = new RtcTest(this);
        connect(rtcTest, &RtcTest::update_btn_style, [=] { testResult(str,rtcTest->result); });
        rtcTest->inintWindow(windSize.width(),windSize.height());
        // qDebug()<<str;

    } else if(str == "PWM"){
        PwmTest *pwmtest = new PwmTest(this);
        connect(pwmtest, &PwmTest::update_btn_style, [=] { testResult(str,pwmtest->result); });
        pwmtest->initWIndow(windSize.width(),windSize.height());
        qDebug()<<str;

    } else if(str == "补光灯"){
        LedTest *ledTest = new LedTest(this);
        connect(ledTest, &LedTest::update_btn_style, [=] { testResult(str,ledTest->result); });
        ledTest->initWindow(windSize.width(),windSize.height());
        qDebug()<<str;

    } else if(str == "继电器"){
        RelayTest *relayTest = new RelayTest(this);
        connect(relayTest, &RelayTest::update_btn_style, [=] { testResult(str,relayTest->result); });
        relayTest->initWindow(windSize.width(),windSize.height());
        qDebug()<<str;
    }else if(str == "自动测试"){
        qDebug()<<"auto test click start";
        this->autoTest();
	
    }else if(str == "RGB"){
        RGBTest *rgbTest = new RGBTest(this);
        connect(rgbTest, &RGBTest::update_btn_style, [=] { testResult(str,rgbTest->result); });
        rgbTest->initWindow(windSize.width(),windSize.height());

    }else if(str == "DDR和EMMC"){
        MemTest *memTest = new MemTest(this);
        connect(memTest, &MemTest::update_btn_style, [=] { testResult(str,memTest->result); });
        memTest->initWindow(windSize.width(),windSize.height());
    }else if (str == "USB摄像头"){
        CameraTest *cameratest = new CameraTest(this);
        connect(cameratest, &CameraTest::update_btn_style, [=] { testResult(str,cameratest->result); });
        cameratest->initWindow(windSize.width(),windSize.height());
    }
}

void MainWindow::testResult(QString btnTxt, bool testResult)
{
    QList<QPushButton *> buttons = centralWidget()->findChildren<QPushButton *>();
    for (QPushButton *btn : buttons) {
        if(btn->text() == btnTxt){
            if (testResult)
                btn->setStyleSheet(btn->styleSheet()+"QPushButton{border-image:url(:/icon/pass.png)}");
            else
                btn->setStyleSheet(btn->styleSheet()+"QPushButton{border-image:url(:/icon/faild.png)}");
        }
        btn->update();
    }

    // for auto test
    if(autoTestFlag && btnTxt == stack.top()){
        disconnect(timer, 0, 0 , 0);
        stack.pop();
        if( stack.isEmpty()){
            autoTestFlag = 0;
            timer->stop();
            return;
        }
        // testClicked(stack.top());
        connect(timer, &QTimer::timeout, [=] {testClicked(stack.top());});
        timer->start(1000);
    }
}

QString MainWindow::getCNName(int groupIndex)
{
    return funLists[groupIndex];
}

void MainWindow::autoTest()
{
    qDebug()<<"auto test click enter";
    autoTestFlag = 1;
    //    int length = sizeof(funLists) / sizeof(funLists[0]);
    qDebug()<<"get arry length";

    QSettings  *setting = new QSettings(CONFIG_FILE_PATH,QSettings::IniFormat);
    QStringList groups = setting->childGroups();

    for(int i = 0;i < groups.length(); i++){
        QString enable = setting->value("/"+groups.at(i)+"/enable").toString();
        int id = setting->value("/"+groups.at(i)+"/id").toInt();
        if(enable == "1"){
            QString fun = getCNName(id - 1);

            if( (fun == "自动测试" ) || (fun =="恢复出厂设置") || (fun  == "重启测试")  || (fun == "otg/host") || (fun == "GPIO"))
                continue;
            qDebug()<<fun;
            stack.push(fun);
        }
    }

    //push to stack
    //    for(int i = length-1; i>=0; i--){
    //       if((funLists[i] != "GPIO") &&
    //               (funLists[i] != "恢复出厂设置") &&
    //               (funLists[i] != "重启测试") &&
    //               (funLists[i] != "自动测试") &&
    //               (funLists[i] != "otg/host"))

    //       stack.push(funLists[i]);
    //    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=] {testClicked(stack.top());});
    timer->start(100);
    //    testClicked(stack.top());
}

MainWindow::~MainWindow()
{
}

