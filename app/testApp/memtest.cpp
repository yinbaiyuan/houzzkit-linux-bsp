#include "memtest.h"
#include <QLayout>

static CommonComponent componet;

MemTest::MemTest(QObject *parent) : QObject(parent)
{

}

void MemTest::initWindow(int screenWidth, int screenHeight){
    widget = componet.getWidget();
    QGridLayout *layout = new QGridLayout;

    tips = componet.getButton("自动读取DDR和EMMC的大小，请核对是否和硬件一致！");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");
    ddrLabel = componet.getLabel("DDR:");
    mmcLabel = componet.getLabel("EMMC:");
    ddrSize = componet.getLabel("");
    mmcSize = componet.getLabel("");
    passBtn = componet.getButton("测试成功");
    faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");

    connect(passBtn, &QPushButton::clicked, [=] {this->getTestResult(true);});
    connect(faildBtn, &QPushButton::clicked, [=] {this->getTestResult(false);});

    layout->addWidget(tips, 1, 1, 1, 2);
    layout->addWidget(ddrLabel, 2, 1, 1, 1, Qt::AlignRight);
    layout->addWidget(ddrSize, 2, 2, 1, 1, Qt::AlignLeft);
    layout->addWidget(mmcLabel, 3, 1, 1, 1, Qt::AlignRight);
    layout->addWidget(mmcSize, 3, 2, 1, 1, Qt::AlignLeft);
    layout->addWidget(passBtn, 4, 1, 1, 1);
    layout->addWidget(faildBtn, 4, 2, 1, 1);

    widget->setLayout(layout);
    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();

    timer = new QTimer(widget);
    connect(timer, &QTimer::timeout, [=]  {this->getMemInfo();});
    timer->start(50);

}

void MemTest::getMemInfo(){
    timer->stop();
    process.start("free -m");
    process.waitForFinished(1000);
    QString str = process.readAll();
//    qDebug()<<str;
    QString memSize = str.replace(QRegExp("\\s{1,}"), " ").split(" ").at(8);
//    qDebug()<<memSize;
    ddrSize->setText(memSize + "M");

    QString usb_dev = componet.getConfig("EMMC_DDR","usb_dev");
    QString sd_dev = componet.getConfig("EMMC_DDR","sd_dev");

    int mmcTotalSize = 0;
    foreach(const QStorageInfo & storage, QStorageInfo::mountedVolumes())
       {
           if (storage.isValid() && storage.isReady()) {
               //qDebug()<<"device"<<QString(storage.device());
               QString device = QString(storage.device());
               if(device.startsWith(usb_dev) || device.startsWith(sd_dev))
                   continue;
               mmcTotalSize += storage.bytesTotal() / 1024 / 1024;
           }
       }

    mmcSize->setText(QString::number(mmcTotalSize) + "M");
}

void MemTest::getTestResult(bool testResult){
    result = testResult;
    widget->close();
    emit update_btn_style();
}
