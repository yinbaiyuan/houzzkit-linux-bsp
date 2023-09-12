#include "wifitest.h"
#include "commoncomponent.h"
#include "QListWidget"
#include "QProcess"
#include "QDebug"
#include "QFile"
#include "stdlib.h"
#include <QTimer>

static CommonComponent componet;
static QListWidget *listWidget;
static QLineEdit *ssidEdit;
static QLineEdit *passwdEdit;
static QLabel *tipLabel;
static QProcess process;
static QTimer *timer;
static QTimer *timer2;
static QTimer *timer3;
static QTimer *timer4;
static int scanCount = 0;

QPushButton *scanBtn;
QPushButton *openBtn;
QPushButton *tips;

static QString openWifiCMD = "echo 1 > /sys/class/rfkill/rfkill1/state && ifconfig wlan0 up";
//static QString scanWIfiCMD = "iwlist wlan0 scan";
//static QString scanWIfiCMD = "wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan && wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan_result";

WifiTest::WifiTest(QObject *parent) : QObject(parent)
{
//    initWindow();
}

void WifiTest::initWindow(int screenWidth, int screenHeight)
{
    widget = componet.getWidget();
    timer = new QTimer(widget);
    timer2 = new QTimer(widget);
    timer3 = new QTimer(widget);
    timer4 = new QTimer(widget);
    connect(timer4, &QTimer::timeout, [=] {this->getTestResult();});
    connect(timer, &QTimer::timeout, [=] { killNotNeed();});

    scanCount = 0;

    listWidget = new QListWidget();
    openBtn = componet.getButton("打开");
    scanBtn = componet.getButton("扫描");
//    QLabel *ssidLabel = componet.getLabel("SSID:");
    ssidEdit = componet.getLineEdit();
    ssidEdit->setDragEnabled(true);
    ssidEdit->setText("RP526");
//    QLabel *passwdLabel = componet.getLabel("密码:");
    passwdEdit = componet.getLineEdit();
    passwdEdit->setEchoMode(QLineEdit::EchoMode::Password);
    passwdEdit->setText("rp778899aa");
    QPushButton *connectBtn = componet.getButton("连接");
    connectBtn->setDisabled(false);
    tipLabel = componet.getLabel("");
    tipLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    connect(openBtn, &QPushButton::clicked, [=] { openWIfi(); });
    connect(scanBtn, &QPushButton::clicked, [=] { scanWifi(); });
    connect(listWidget, &QListWidget::itemClicked, [=] {ssidEdit->setText(listWidget->currentItem()->text()); });
    connect(connectBtn, &QPushButton::clicked, [=] { connectWifi(ssidEdit->text(), passwdEdit->text()); });

    QLabel *scanLabel = componet.getLabel("扫描结果");
    QGridLayout *layout = new QGridLayout();

    tips = componet.getButton("正在自动测试wifi功能，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    layout->addWidget(tips, 1, 1, 1, 4);
    layout->addWidget(openBtn, 2, 1, 1, 2);
    layout->addWidget(scanBtn, 2, 3, 1, 2);
    layout->addWidget(scanLabel, 3, 1, 1, 4);
    layout->addWidget(listWidget, 4, 1, 8, 4);
//    layout->addWidget(ssidLabel, 12, 1);
//    layout->addWidget(ssidEdit, 12, 2, 1, 3);
//    layout->addWidget(passwdLabel, 13, 1);
//    layout->addWidget(passwdEdit, 13, 2, 1, 3);
//    layout->addWidget(connectBtn, 14, 2, 1, 2);
//    layout->addWidget(tipLabel, 15, 1, 1, 4);

    widget->setLayout(layout);
//    widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);

    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();
    timer->start(100);
}

void WifiTest::openWIfi()
{
    timer2->stop();
    ::system(openWifiCMD.toLocal8Bit());
//    qDebug()<<"open success==========================";
    openBtn->setText("已打开");

    connect(timer3, &QTimer::timeout, [=] { this->scanWifi(); });
    scanBtn->setText("正在扫描");
    timer3->setInterval(1500);
    timer3->start();
}

void WifiTest::scanWifi()
{
    QString sRet;
    scanCount ++;
    ::system(QString("wpa_cli -i wlan0 scan").toLocal8Bit());
    listWidget->clear();

    ::system(QString("wpa_cli -i wlan0 scan_result > /userdata/wifi_scanlist").toLocal8Bit());

    QFile scan_result("/userdata/wifi_scanlist");
    scan_result.open(QIODevice::ReadWrite);

    sRet = scan_result.readAll();

    QList<QString> resultList = sRet.replace(" ","").replace("bssid/frequency/signallevel/flags/ssid\n","").split("\n");
    if(resultList.length() < 2){
        if(scanCount > 3){
            timer3->stop();
            tips->setText("正在自动测试wifi功能，测试失败！！");
            result=false;
            timer4->start(2000);
        }
        return;
    }else {
        for(QString str : resultList){
            str.remove(0,str.lastIndexOf("\t")+1);
            str = str.replace(" ","");
            if(str != "" && str != NULL)
                listWidget->addItem(str);
        }
        listWidget->update();
    }

    scan_result.close();
    scan_result.remove();
    tips->setText("正在自动测试wifi功能，测试成功！！");
    timer3->stop();
    result=true;
    timer4->start(2000);
}

//keep it
void WifiTest::setWifiInfo(QString ssid)
{
//    ssidEdit->setText(ssid);
}

void WifiTest::connectWifi(QString SSID, QString passwd)
{
    QString sRet;
    tipLabel->setText("");
    if(SSID == "" || passwd.length() < 8){
        tipLabel->setText("SSID或密码输入错误！");
        return;
    }

    QFile connectBash("/usr/bin/wifi_start.sh");
    if(connectBash.exists()){
        QString connectCmd = QString("wifi_start.sh %1 %2 > /userdata/wifi_connect_status").arg(SSID).arg(passwd);
        ::system(connectCmd.toLocal8Bit());

//        process.start(connectCmd);
//        process.waitForReadyRead(2000);
//        process.waitForFinished(10000);
        QFile wifi_connect_status("/userdata/wifi_connect_status");
        if(!wifi_connect_status.open(QIODevice::ReadWrite)){
            tipLabel->setText("连接失败！");
            return;
        }

//      sRet = process.readAll();

        sRet = wifi_connect_status.readAll();
        wifi_connect_status.close();

        qDebug()<<"return result:"<<sRet;

        if(sRet.indexOf("Successfully") >= 0){

//            process.start("udhcpc -i wlan0");
//            process.waitForReadyRead(10000);
//            process.waitForFinished(10000);
            ::system("udhcpc -i wlan0 > /userdata/wifi_connect_status");

            QFile wifi_connect_status("/userdata/wifi_connect_status");
            if(!wifi_connect_status.open(QIODevice::ReadWrite)){
                tipLabel->setText("连接失败！");
                return;
            }
//            sRet = process.readAll();
            sRet = wifi_connect_status.readAll();
            qDebug()<<"udhcpc return result:"<<sRet;
            if(sRet.indexOf("deleting routers") >= 0  ||sRet.indexOf("adding dns") >= 0 )
                tipLabel->setText("连接成功！");
            else
                tipLabel->setText("连接失败！");

            process.kill();
            //TODO 获取wlan0的ip地址
        }else{
            tipLabel->setText("连接失败！");
        }

        wifi_connect_status.close();
        wifi_connect_status.remove();
    }else{
        //nothing todo 这里供其他没有这个连接脚本的平台使用
    }
}

void WifiTest::killNotNeed(){
    timer->stop();
    QFile file("/oem/RkLunch-stop.sh");
    if(file.exists()){
        ::system(QString("sh /oem/RkLunch-stop.sh").toLocal8Bit());
    }

//    ::system(QString("killall wpa_supplicant").toLocal8Bit());

    QString wpa_config_file = componet.getConfig("wifi","wpa_config");
//    qDebug()<<wpa_config_file;

    ::system(QString("wpa_supplicant -B -i wlan0 -c %1").arg(wpa_config_file).toLocal8Bit());

    connect(timer2, &QTimer::timeout, [=] { this->openWIfi(); });
    openBtn->setText("正在打开");
    timer2->start(100);
}

void WifiTest::getTestResult(){
    emit update_btn_style();
    this->widget->close();
}

