#include "tfcardtest.h"
#include "commoncomponent.h"
static CommonComponent componet;

TFcardTest::TFcardTest(QObject *parent) : QObject(parent)
{

}

void TFcardTest::initWindow(int winWidth, int winHeight){
    widget = componet.getWidget();
    timer = new QTimer(widget);

    tips = componet.getButton("正在自动测试TF/SD卡，请稍候。。。");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(tips);
    widget->setLayout(layout);
//  widget->move((winWidth-widget->width())/2, (winHeight-widget->height())/2);

    connect(timer, &QTimer::timeout, [=] {getTestResult();});
    widget->resize(winWidth,winHeight);
    widget->showFullScreen();
    timer->start(1000);

}

void TFcardTest::getTestResult()
{
//    QFile file1("/dev/mmcblk1");
//    QFile file0("/dev/mmcblk0");
//    QFile file2("/dev/mmcblk2");

    QString str = componet.getConfig("tfcard", "sd_dev");
    QFile sd_dev(str);
    if(sd_dev.exists()){
        result = true;
        tips->setText("正在自动测试TF/SD卡，测试成功！");
    }else{
        result = false;
        tips->setText("正在自动测试TF/SD卡，测试失败！");
    }

//    if(file0.exists() && (file1.exists() || file2.exists())){
//        result = true;
//        tips->setText("正在自动测试TF/SD卡，测试成功！");
//    }else{
//        result = false;
//        tips->setText("正在自动测试TF/SD卡，测试失败！");
//    }
    connect(timer, &QTimer::timeout, [=] {widget->close();});
    timer->start(1000);
    emit update_btn_style();
}

