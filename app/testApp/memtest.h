#ifndef MEMTEST_H
#define MEMTEST_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QStorageInfo>
#include "commoncomponent.h"

class MemTest : public QObject
{
    Q_OBJECT
public:
    explicit MemTest(QObject *parent = nullptr);

    void initWindow(int screenWidth, int screenHeight);

    QWidget *widget;
    QPushButton *tips;
    QLabel *ddrLabel;
    QLabel *mmcLabel;
    QLabel *ddrSize;
    QLabel *mmcSize;
    QPushButton *passBtn;
    QPushButton *faildBtn;
    QProcess process;
    QTimer *timer;
    bool result;

public slots:
    void getMemInfo();
    void getTestResult(bool testResult);

signals:
    void update_btn_style();

};

#endif // MEMTEST_H
