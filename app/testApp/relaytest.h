#ifndef RELAYTEST_H
#define RELAYTEST_H

#include <QObject>

class RelayTest : public QObject
{
    Q_OBJECT
public:
    explicit RelayTest(QObject *parent = nullptr);
    void initWindow(int winWidth, int winHeight);
    void getTestResult(bool result);
    void handleTimeout(QString rp_gpio);
    void pullRelay(QString status);
    bool result;

signals:
    void update_btn_style();

};

#endif // RELAYTEST_H
