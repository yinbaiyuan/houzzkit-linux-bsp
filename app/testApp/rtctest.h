#ifndef RTCTEST_H
#define RTCTEST_H

#include <QObject>

class RtcTest : public QObject
{
    Q_OBJECT
public:
    explicit RtcTest(QObject *parent = nullptr);

    void inintWindow(int winWidth, int winHeight);

    bool result;

public slots:
    void getTestResult();

signals:
    void update_btn_style();

};

#endif // RTCTEST_H
