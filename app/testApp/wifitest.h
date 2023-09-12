#ifndef WIFITEST_H
#define WIFITEST_H

#include <QObject>
#include <QPushButton>

class WifiTest : public QObject
{
    Q_OBJECT
public:
    explicit WifiTest(QObject *parent = nullptr);

    void initWindow(int screenWidth, int screenHeight);

    QWidget *widget;

    bool result;

public slots:
    void openWIfi();

    void scanWifi();

    void setWifiInfo(QString SSID);

    void connectWifi(QString SSID, QString passwd);

    void killNotNeed();

    void getTestResult();

signals:
    void update_btn_style();

};

#endif // WIFITEST_H
