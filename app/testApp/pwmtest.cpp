#include "pwmtest.h"
#include "commoncomponent.h"
#include "QDir"
#include "QFile"
#include "QDebug"
#include "stdlib.h"
#include <QTimer>

static CommonComponent componet;
static QString devPath = "/sys/class/pwm/";
static int period = 1000;
static int duty_cycle = 700;
static QString model = "normal";
static QTimer *timer;
static QWidget *widget;
static int backlightLevel = 255;
static int sign = 0;
static QSlider *backLight;

PwmTest::PwmTest(QObject *parent) : QObject(parent)
{

}

void PwmTest::initWIndow(int screenWidth, int screenHeight){
    backlightLevel = 255;
    sign = 0;
    QGridLayout *mainLayout = new QGridLayout();

    widget = componet.getWidget();
    timer = new QTimer(widget);
    timer->setInterval(200);

    QDir dir(devPath);
    dir.setFilter(QDir::Dirs);

    QPushButton *tips = componet.getButton("自动改变背光PWM的占空比，请查看屏幕背光是否有亮暗变化！");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    QPushButton *passBtn = componet.getButton("测试成功");
    QPushButton *faildBtn = componet.getButton("测试失败");
    faildBtn->setStyleSheet(faildBtn->styleSheet()+"QPushButton{background-color:#f56c6c}");

    connect(passBtn, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faildBtn, &QPushButton::clicked, [=] { this->getTestResult(false); });

    int i = 1;
    for(QFileInfo info : dir.entryInfoList()){
        if(info.fileName() == "." || info.fileName() == "..")
            continue;

        if(initPwm(info.fileName())){
            i++;
            QLabel *pwmLabel = componet.getLabel(info.fileName());
            QSlider *pwmSlider = new QSlider();
            pwmSlider->setOrientation(Qt::Horizontal);
            pwmSlider->setMaximum(period);
            pwmSlider->setMinimum(0);
            pwmSlider->setSingleStep(10);
            pwmSlider->setValue(duty_cycle);

            connect(pwmSlider, &QSlider::valueChanged, [=] { this->setPwm(info.fileName(), pwmSlider->value()); });

            QHBoxLayout *pwmLayout  = new QHBoxLayout();
            pwmLayout->addWidget(pwmLabel);
            pwmLayout->addWidget(pwmSlider);

            mainLayout->addLayout(pwmLayout, i, 1, 1, 5);
        }
    }

    //backlight
    backLight = new QSlider();
    backLight->setOrientation(Qt::Horizontal);
    backLight->setMaximum(255);
    backLight->setMinimum(1);
    backLight->setValue(200);

    connect(backLight, &QSlider::valueChanged, [=] { this->setBacklight(backLight->value()); });
    connect(timer, &QTimer::timeout, [=] { this->setBacklight(backlightLevel); });
    QLabel *backLightLabel = componet.getLabel("背光:");

    mainLayout->addWidget(tips, 1, 1, 1, 5);
    mainLayout->addWidget(backLightLabel, i+1, 1);
    mainLayout->addWidget(backLight, i+1, 2, 1, 4);
    mainLayout->addWidget(passBtn,i+2,1,1,2);
    mainLayout->addWidget(faildBtn,i+2,4,1,2);

    widget->setLayout(mainLayout);
    //    widget->resize(400,400);
    //    widget->move((screenWidth-widget->width())/2, (screenHeight-widget->height())/2);
    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();
    timer->start();
}


void PwmTest::setPwm(QString pwmName, int duty)
{
    QString pwmPath = devPath + pwmName;
    ::system(QString("echo %1 > %2/pwm0/duty_cycle").arg(QString::number(duty)).arg(pwmPath).toLocal8Bit());
}


void PwmTest::setBacklight(int value)
{
    if(sign == 0){
        backlightLevel -= 10;
        if (backlightLevel <=10)
            sign = 1;
    }else if(sign == 1){
        backlightLevel += 10;
        if (backlightLevel > 240)
            sign = 0;
    }
    backLight->setValue(value);
    ::system(QString(" echo %1 > /sys/class/backlight/backlight/brightness").arg(QString::number(value)).toLocal8Bit());
}

int PwmTest::initPwm(QString pwmName)
{
    QString pwmPath = devPath + pwmName;
    QFile file(pwmPath + "/pwm0");
    if(!file.exists()){

        // for rockchip
        // example:
        // cd /sys/class/pwm/pwmchip0/
        // echo 0 > export
        // cd pwm0
        // echo 10000 > period
        // echo 5000 > duty_cycle
        // echo normal > polarity
        // echo 1 > enable

        ::system(QString("echo 0 > %1/export").arg(pwmPath).toLocal8Bit());
        ::system(QString("echo %1 > %2/pwm0/period").arg(QString::number(period)).arg(pwmPath).toLocal8Bit());
        ::system(QString("echo %1 > %2/pwm0/duty_cycle").arg(QString::number(duty_cycle)).arg(pwmPath).toLocal8Bit());
        ::system(QString("echo %1 > %2/pwm0/polarity").arg(model).arg(pwmPath).toLocal8Bit());
        ::system(QString("echo %1 > %2/pwm0/enable").arg(QString::number(1)).arg(pwmPath).toLocal8Bit());
    }

    if(!file.exists())
        return 0;

    return 1;
}

void PwmTest::getTestResult(bool testResult)
{
    result = testResult;
    emit update_btn_style();
    widget->close();
}





