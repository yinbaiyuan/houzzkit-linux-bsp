#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>

class BlueTooth : public QObject
{
    Q_OBJECT
public:
    explicit BlueTooth(QObject *parent = nullptr);
    void initWindow(int screenWidth, int screenHeight);
    bool result = false;

public slots:
    void openBT();

    void getBTdev();

    void getTestResult();

signals:
    void update_btn_style();

};

#endif // BLUETOOTH_H
