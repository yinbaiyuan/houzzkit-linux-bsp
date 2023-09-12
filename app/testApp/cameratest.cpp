#include "cameratest.h"

static int initStatus = -1;

CameraTest::CameraTest(QObject *parent) : QObject(parent)
{

}

void CameraTest::initWindow(int winWidth, int winHeight)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *layout_h = new QHBoxLayout;
    lable = componet.getLabel("");
    QPushButton *passBtn = componet.getButton("测试成功");
    QPushButton *faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");

    connect(passBtn, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faildBtn, &QPushButton::clicked, [=] { this->getTestResult(false); });

    layout_h->addWidget(passBtn);
    layout_h->addWidget(faildBtn);

    widget = componet.getWidget();
    layout->addWidget(lable);
    layout->addLayout(layout_h);
    widget->setLayout(layout);
    widget->resize(winWidth, winHeight);
    widget->showFullScreen();

    timer = new QTimer(widget);
    if(initStatus == -1 ){

        // get config.ini  configure
        QString file_video = componet.getConfig("USB_camera","video");
        qDebug()<<"video dev is :"<<file_video;
        QByteArray ba;
        ba = file_video.toLatin1();
        char *file_video2 = ba.data();

        initStatus = initCamera(&file_video2);
        if(initStatus !=0 )
            return;

        initStatus = requestMemory();
        if(initStatus !=0 )
            return;

        initStatus = startStream();
        if (initStatus != 0)
            return;
    }else{
        qDebug()<<"camera aready start";
    }
    connect(timer, &QTimer::timeout, [=] { this->disply(); });
    timer->start(1000/30);
}

void CameraTest::disply()
{
    QImage img;
    getFrame();
    img = QImage(rgb24, 640, 480, QImage::Format_RGB888);
    lable->setPixmap(QPixmap::fromImage(img));
}

void CameraTest::getTestResult(bool testResult)
{
    result = testResult;
    widget->close();
    emit update_btn_style();
}


