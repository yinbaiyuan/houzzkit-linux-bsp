#ifndef TFCARDTEST_H
#define TFCARDTEST_H

#include <QObject>
#include <QTimer>
#include <QPushButton>

class TFcardTest : public QObject
{
    Q_OBJECT
public:
    explicit TFcardTest(QObject *parent = nullptr);
    void initWindow(int winWidth, int winHeight);

    QWidget *widget;
    QTimer *timer;
    QPushButton *tips;
    bool result;

public slots:
    void getTestResult();
signals:
    void update_btn_style();

};

#endif // TFCARDTEST_H
