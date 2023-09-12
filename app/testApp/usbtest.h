#ifndef USBTEST_H
#define USBTEST_H

#include <QObject>
#include <QTimer>
#include <QPushButton>

class USBTest : public QObject
{
    Q_OBJECT
public:
    explicit USBTest(QObject *parent = nullptr);

    void initWindow(int w, int h);

    QWidget *widget;

    QTimer *timer;

    bool result;

    QPushButton *tips;

public slots:
    void getTestResult();
signals:
    void update_btn_style();

};

#endif // USBTEST_H
