#ifndef COMMONCOMPONENT_H
#define COMMONCOMPONENT_H
#include "QPushButton"
#include "QLabel"
#include "QTextEdit"
#include "QLayout"
#include "QComboBox"
#include "QLCDNumber"
#include "QLineEdit"
#include "QRadioButton"
#include <QDir>
#include <QSettings>

#define CONFIG_FILE_PATH "/etc/config.ini"

class CommonComponent : QObject
{
    Q_OBJECT
public:
    CommonComponent();

    QPushButton* getButton(QString str);

    QLabel* getLabel(QString str);

    QWidget* getWidget();

    QTextEdit* getTextEdit();

    QComboBox* getComboBox();

    QLCDNumber* getLcdNumber();

    QLineEdit* getLineEdit();

    QHBoxLayout* getTitleBar(QWidget *widget, QString);

    QRadioButton* getRadioButton(QString btnTxt);

    bool checkConfig(QString config);

    QString getConfig(QString group, QString attribute);
};

#endif // COMMONCOMPONENT_H
