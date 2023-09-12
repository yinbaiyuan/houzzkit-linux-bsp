#ifndef OTG_HOST_H
#define OTG_HOST_H

#include <QObject>

class Otg_Host : public QObject
{
    Q_OBJECT
public:
    explicit Otg_Host(QObject *parent = nullptr);

    void initWindow(int winWidth, int winHeight);

    void changMode(QString mode, QString rp_gpio);

    void handleTimeout(bool testResult);

    bool result;

    QString rp_gpio;
    QString otg_dev_str;
    QString host_dev_str;

signals:

    void update_btn_style();

};

#endif // OTG_HOST_H
