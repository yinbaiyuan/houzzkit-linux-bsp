#ifndef AUTOREBOOT_H
#define AUTOREBOOT_H

#include <QObject>

class AutoReboot : public QObject
{
    Q_OBJECT
public:
    explicit AutoReboot(QObject *parent = nullptr);
    void initWindow();

public slots:
    void stopReboot();
    void startReboot();

signals:

};

#endif // AUTOREBOOT_H
