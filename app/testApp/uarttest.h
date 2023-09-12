#ifndef UARTTEST_H
#define UARTTEST_H
#include <qobject.h>
#include <QSerialPort>

class UartTest:public QObject
{
    Q_OBJECT

public:
    explicit UartTest(QObject *parent = nullptr);
    void showWindow(int screenWidth, int screenHeight);
    bool result;

public slots:
    void sendSerialData(QString message, QSerialPort *testSerial);

    void reciveData(QSerialPort *testSerial);

    void changeSerial(QString serial);

    void getTestResult();

signals:
    void update_btn_style();

};

#endif // UARTTEST_H
