#ifndef PWMTEST_H
#define PWMTEST_H

#include <QObject>

class PwmTest : public QObject
{
    Q_OBJECT
public:
    explicit PwmTest(QObject *parent = nullptr);

    void initWIndow(int screenWidth, int screenHeight);

    int initPwm(QString pwmName);

    void setBacklight(int value);

    bool result;

public slots:
    void setPwm(QString pwmName, int duty);
    void getTestResult(bool testResult);

signals:
    void update_btn_style();

};

#endif // PWMTEST_H
