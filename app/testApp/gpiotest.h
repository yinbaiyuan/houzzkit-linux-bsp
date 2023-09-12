#ifndef GPIOTEST_H
#define GPIOTEST_H

#include <QObject>

class GpioTest : public QObject
{
    Q_OBJECT
public:
    explicit GpioTest(QObject *parent = nullptr);

    void initWindow(int screenWidth, int screenHeight);

    int convertGpio(QString gpioStr);

public slots:
    void setValToHigh(int value);

    void setValToLow(int value);

    void readValue();

signals:

};

#endif // GPIOTEST_H
