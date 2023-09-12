#ifndef _4GTEST_H
#define _4GTEST_H

#include <QObject>

class _4GTest : public QObject
{
    Q_OBJECT
public:
    explicit _4GTest(QObject *parent = nullptr);

    void initWindow(int screenWidth, int screenHeight);

    bool result;

public slots:
    void call();

    void test(QString targetAddr);

    void delyCall();

    void readMessage();

    void getTestResult();

signals:
    void update_btn_style();

};

#endif // _4GTEST_H
