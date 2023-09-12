#include "ethnettest.h"
#include "commoncomponent.h"
#include "QLayout"
#include "QHostAddress"
#include "QNetworkInterface"
#include "QNetworkConfigurationManager"
#include "QProcess"
#include "QApplication"
#include <QTimer>
#include <QHostInfo>

static CommonComponent componet;
static QWidget *widget;
static QTextEdit *textEdit;
static QProcess process;
static QLineEdit *targetIp;
static QTimer *timer;
static QString activeEth;

EthnetTest::EthnetTest(QObject *parent) : QObject(parent)
{
    //    initWindow();
}

void EthnetTest::initWindow(int screenWidth, int screenHeight)
{
    activeEth.clear();
    widget = componet.getWidget();
    ::system("ifconfig eth0 up");
    timer = new QTimer(widget);
    QVBoxLayout *layout_v = new QVBoxLayout();
    QGridLayout *gridLayout = new QGridLayout();
    textEdit = componet.getTextEdit();

    QTimer *checkEth = new QTimer(widget);
    connect(checkEth, &QTimer::timeout,[=] { getReticleStat();});
    checkEth->setInterval(500);
    checkEth->start();

    //    int i = 0;
    //    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    qDebug()<<ipAddressesList;
    //    if( ipAddressesList.size() >0 ){
    //        for(QHostAddress address : ipAddressesList){
    //            if (address != QHostAddress::LocalHost && address.toIPv4Address() && address.toString().startsWith("192.168")) {
    //                i++;
    //                //qDebug()<<address.toString();
    //                QLabel *ethLabel = componet.getLabel("主板IP"+ QString::number(i) +":");
    //                QLineEdit *ipaddr = componet.getLineEdit();
    //                ipaddr->setText(address.toString());
    //                ipaddr->setDisabled(true);
    //                QPushButton *pingBtn = componet.getButton("ping");

    //                connect(pingBtn, &QPushButton::clicked, [=] { netWorkTest(address.toString()); });
    //                //                connect(timer, &QTimer::timeout, [=] { netWorkTest(address.toString());});

    //                gridLayout->addWidget(ethLabel,i,1);
    //                gridLayout->addWidget(ipaddr,i,2,1,2);
    //                gridLayout->addWidget(pingBtn,i,4,1,1);
    //            }
    //        }
    //    }

    //    if(i == 0){
    //        QLabel *noIp = componet.getLabel("获取IP地址失败，请检查网线连接！");
    //        gridLayout->addWidget(noIp, i+1, 1, 1, 5);
    //        i++;
    //        textEdit->append("测试失败！");
    //    }

    QLabel *ethLabel1 = componet.getLabel("eth0:");
    ethLabel1->setObjectName("ethLabel_eth0");
    QLineEdit *ipaddr1 = componet.getLineEdit();
    ipaddr1->setObjectName("ipaddr_eth0");
    QPushButton *pingBtn1 = componet.getButton("ping");
    pingBtn1->setObjectName("pingBtn_eth0");

    ethLabel1->setVisible(false);
    ipaddr1->setVisible(false);
    pingBtn1->setVisible(false);

    QLabel *ethLabel2 = componet.getLabel("eth1:");
    ethLabel2->setObjectName("ethLabel_eth1");
    QLineEdit *ipaddr2 = componet.getLineEdit();
    ipaddr2->setObjectName("ipaddr_eth1");
    QPushButton *pingBtn2 = componet.getButton("ping");
    pingBtn2->setObjectName("pingBtn_eth1");

    ethLabel2->setVisible(false);
    ipaddr2->setVisible(false);
    pingBtn2->setVisible(false);

    connect(pingBtn1, &QPushButton::clicked, [=] { netWorkTest("eth0"); });
    connect(pingBtn2, &QPushButton::clicked, [=] { netWorkTest("eth1"); });

    QLabel *testLabel = componet.getLabel("测试IP:");
    targetIp = componet.getLineEdit();
    targetIp->setText("192.168.1.1");
    QLabel *resultLaber = componet.getLabel("测试结果：");


    gridLayout->addWidget(ethLabel1, 1, 1);
    gridLayout->addWidget(ipaddr1, 1, 2, 1, 2);
    gridLayout->addWidget(pingBtn1, 1, 4, 1, 1);

    gridLayout->addWidget(ethLabel2, 2, 1);
    gridLayout->addWidget(ipaddr2, 2, 2, 1, 2);
    gridLayout->addWidget(pingBtn2, 2, 4, 1, 1);

    gridLayout->addWidget(testLabel,3, 1);
    gridLayout->addWidget(targetIp,3 ,2, 1, 3);

    QPushButton *tips = componet.getButton("正在自动测试以太网通信，请稍后...");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QHBoxLayout *layout_h = new QHBoxLayout;
    QPushButton *passBtn = componet.getButton("测试成功");
    QPushButton *faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");
    connect(passBtn, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faildBtn, &QPushButton::clicked, [=] { this->getTestResult(false); });
    layout_h->addWidget(passBtn);
    layout_h->addWidget(faildBtn);

    layout_v->addWidget(tips);
    layout_v->addLayout(gridLayout);
    layout_v->addWidget(resultLaber);
    layout_v->addWidget(textEdit);
    layout_v->addLayout(layout_h);
    //    layout_v->setMargin(0);
    //    layout_v->setSpacing(6);

    //    timer->start(1000);
    widget->setLayout(layout_v);
    // widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);
    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();
}

void EthnetTest::netWorkTest(QString ipaddress){
    timer->stop();
    //    textEdit->clear();
    if(!ipaddress.isEmpty()){
        QString sCmd = QString("ping -I %1 -s 1 -c 2 %2").arg(ipaddress).arg(targetIp->text());
        qDebug()<<sCmd;
        process.start(sCmd);
        process.waitForReadyRead(500);
        process.waitForFinished(500);
        QString sRet = process.readAll();
        process.close();
        textEdit->append(sRet.toUtf8());

        if(sRet.indexOf("ttl") >= 0){
            textEdit->append(ipaddress+"测试成功！");
            result = true;
        }else{
            result = false;
            textEdit->append(ipaddress+"测试失败！");
        }
    }else{
        result = false;
        textEdit->append(ipaddress+"测试失败！");
    }

    //    connect(timer, &QTimer::timeout, [=] { widget->close();});
    //    timer->start(1000);
    emit update_btn_style();
}

void EthnetTest::getReticleStat()
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();//获取所有网卡接口信息到list
    //遍历接口信息
    foreach(QNetworkInterface interface,list){
        int i = 0;
        QNetworkInterface::InterfaceFlags flags = interface.flags();//获取flag

        if(QNetworkInterface::Ethernet == interface.type()){

            if(flags.testFlag(QNetworkInterface::IsUp)){ //判断活动状态，可以此检测网线插拔
                if(activeEth != interface.name()){
                    // qDebug()<<interface.name()<<"is up";
                    QString cmd = QString("timeout 2 udhcpc -i %1").arg(interface.name());
                    system(cmd.toLocal8Bit());

                    QList<QNetworkAddressEntry> iplist = interface.addressEntries();//获取当前ip

                    if(iplist.size() > 1){
                        //成功获取到IP
                        activeEth = interface.name();
                        textEdit->append(interface.name() + "已连接");

                        qDebug()<<"interface name:"<<interface.name();

                        QLineEdit* ipaddress =widget->findChild<QLineEdit*>("ipaddr_" + interface.name());
                        QPushButton* ping =widget->findChild<QPushButton*>("pingBtn_" + interface.name());
                        QLabel* label =widget->findChild<QLabel*>("ethLabel_" + interface.name());

                        ipaddress->setText(iplist.at(0).ip().toString());

                        ipaddress->setVisible(true);
                        ping->setVisible(true);
                        label->setVisible(true);

                        netWorkTest(interface.name());

                    }else{
                        // 获取失败
//                        textEdit->append(interface.name() + "已连接,但获取ip失败！");
                    }
                }

            }else{
                if (interface.name() == activeEth){
                    activeEth="";
                    // qDebug()<<interface.name()<<"is disconnect!";
                    textEdit->append(interface.name() + "已断开");
                    QLineEdit* ipaddress =widget->findChild<QLineEdit*>("ipaddr_" + interface.name());
                    QPushButton* ping =widget->findChild<QPushButton*>("pingBtn_" + interface.name());
                    QLabel* label =widget->findChild<QLabel*>("ethLabel_" + interface.name());

                    //ipaddress->setText(iplist.at(0).ip().toString());
                    ipaddress->setVisible(false);
                    ping->setVisible(false);
                    label->setVisible(false);
                }
            }
        }
    }
}

void EthnetTest::getTestResult(bool testResult)
{
    result = testResult;
    emit update_btn_style();
    widget->close();
}

EthnetTest::~EthnetTest()
{
}
