#include "rgbtest.h"
#include "commoncomponent.h"

static CommonComponent componet;
static int times = 0;
static QString rgb[] = {"QWidget{background-color:red}","QWidget{background-color:green}","QWidget{background-color:blue}"};

RGBTest::RGBTest(QObject *parent) : QObject(parent)
{

}

void RGBTest::initWindow(int winWidth, int winHeight)
{
    times = 0;
    widget = componet.getWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *layout_v = new QVBoxLayout;

    QLabel *lable = componet.getLabel("检查屏幕颜色是否正常");
    passBtn = componet.getButton("测试成功");
    passBtn->setVisible(false);
    faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet() + "QPushButton{background-color:#f56c6c}");
    faildBtn->setVisible(false);
    connect(passBtn, &QPushButton::clicked, [=] {getTestResult(true);});
    connect(faildBtn, &QPushButton::clicked, [=] {getTestResult(true);});

    layout_v->addWidget(lable);
    layout->setAlignment(lable, Qt::AlignCenter);
    layout->addWidget(passBtn);
    layout->addWidget(faildBtn);
    layout_v->addLayout(layout);

    widget->setLayout(layout_v);
    widget->resize(winWidth,winHeight);
    widget->showFullScreen();

    timer = new QTimer(widget);
    connect(timer, &QTimer::timeout, [=] {changeBgColor();});
    timer->setInterval(1500);
    timer->start();

}

void RGBTest::changeBgColor()
{
    widget->setStyleSheet(widget->styleSheet() + rgb[times]);
    times ++ ;
    if(times > 2){
        timer->stop();
        passBtn->setVisible(true);
        faildBtn->setVisible(true);
    }
}

void RGBTest::getTestResult(bool testResult)
{
    result = testResult;
    widget->close();
    emit update_btn_style();
}
