#ifndef LEDTEST_H
#define LEDTEST_H

#include <QObject>

class LedTest : public QObject
{
    Q_OBJECT
public:
    explicit LedTest(QObject *parent = nullptr);
    void initWindow(int winWidth, int winHeight);
    bool result;

public slots:
    void setValue(QString ledName, int value);
    void getTestResult(bool testResult);
    void handleTimeout();
signals:
    void update_btn_style();

};

#endif // LEDTEST_H
