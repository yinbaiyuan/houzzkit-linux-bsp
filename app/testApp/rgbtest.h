#ifndef RGBTEST_H
#define RGBTEST_H
#include <QObject>
#include <QTimer>
#include <QPushButton>

class RGBTest : public QObject
{
    Q_OBJECT
public:
   explicit RGBTest(QObject *parent = nullptr);

    void initWindow(int winWidth, int winHeight);

    QWidget *widget;

    QTimer *timer;

    QPushButton *passBtn;

    QPushButton *faildBtn;

    bool result;

public slots:
    void changeBgColor();

    void getTestResult(bool testResult);

signals:
    void update_btn_style();
};

#endif // RGBTEST_H
