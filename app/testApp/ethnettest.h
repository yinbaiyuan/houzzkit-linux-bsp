#ifndef ETHNETTEST_H
#define ETHNETTEST_H

#include <QObject>

class EthnetTest : public QObject
{
    Q_OBJECT
public:
    explicit EthnetTest(QObject *parent = nullptr);
    ~EthnetTest();
    void initWindow(int screenWidth, int screenHeight);
    bool result;

public slots:
    void netWorkTest(QString ipaddress);
    void getTestResult(bool testResult);
    void getReticleStat();

signals:
    void update_btn_style();

};

#endif // ETHNETTEST_H
