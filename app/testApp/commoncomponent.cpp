#include "commoncomponent.h"

CommonComponent::CommonComponent()
{
}

QPushButton* CommonComponent::getButton(QString str)
{
    QPushButton *button = new QPushButton(str);
    QFont btnFont;
    // btnFont.setPointSize(14);
    btnFont.setPixelSize(14);
    button->setFocusPolicy(Qt::NoFocus);
    button->setStyleSheet("QPushButton {width:110;height:45;background-color:#409eff;color:white;border-radius:2}QPushButton:pressed {background-color:#3a8ee6}");
    button->setFont(btnFont);
    return button;
}

QLabel* CommonComponent::getLabel(QString str)
{
    QLabel *label = new QLabel(str);
    QFont font;
    // font.setPointSize(14);
    font.setPixelSize(14);
//    label->setAlignment(Qt::AlignRight);
    label->setFont(font);
    return label;
}

QTextEdit* CommonComponent::getTextEdit()
{
    QTextEdit *textEdit = new QTextEdit();
    QFont font;
    font.setPixelSize(14);
    textEdit->setFont(font);
    return textEdit;
}

QWidget* CommonComponent::getWidget()
{
    QWidget *widget = new QWidget();
    widget->setObjectName("widgetMain");
    widget->setStyleSheet("QWidget#widgetMain{border: 1px solid #07a5ff; border-radius: 2px;}");
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    widget->setContentsMargins(5,0,5,5);
    widget->setWindowModality(Qt::ApplicationModal);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    return widget;
}

QComboBox* CommonComponent::getComboBox()
{
    QComboBox *comboBox = new QComboBox();
    return comboBox;
}

QLCDNumber* CommonComponent::getLcdNumber()
{
    QLCDNumber *lcdNumber = new QLCDNumber();
    lcdNumber->setDigitCount(2);
    return lcdNumber;
}

QLineEdit* CommonComponent::getLineEdit()
{
    QLineEdit *lineEdit = new QLineEdit();
    QFont font;
    font.setPixelSize(14);
    lineEdit->setStyleSheet("QLineEdit{height:43;border:1px solid #409eff}");
    lineEdit->setFont(font);
    return lineEdit;
}

QHBoxLayout* CommonComponent::getTitleBar(QWidget *widget, QString title)
{
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleTxt = getLabel(title);
    QPushButton *closeBtn = getButton("关闭");
    closeBtn->setFixedSize(40,20);
    connect(closeBtn, &QPushButton::clicked, [=] { widget->close(); });

    titleLayout->addWidget(titleTxt);
    titleLayout->addWidget(closeBtn);
    titleLayout->setAlignment(closeBtn,Qt::AlignRight);
    titleLayout->setAlignment(titleTxt,Qt::AlignLeft);
    return titleLayout;
}

QRadioButton* getRadioButton(QString btnTxt)
{
    QRadioButton *radioButton = new QRadioButton();
    radioButton->setText(btnTxt);
    return radioButton;
}

bool CommonComponent::checkConfig(QString config)
{
    QDir dir("/proc/rp_gpio/");
    foreach(QFileInfo file, dir.entryInfoList()){
        if(file.fileName() == config){
            return true;
        }
    }
    return false;
}

QString CommonComponent::getConfig(QString group, QString attribute)
{
    QSettings  *setting = new QSettings(CONFIG_FILE_PATH,QSettings::IniFormat);
    return setting->value("/" + group + "/" + attribute).toString();
}


