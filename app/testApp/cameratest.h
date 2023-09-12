#ifndef CAMERATEST_H
#define CAMERATEST_H

#include <QObject>
#include "commoncomponent.h"
#include <QTimer>
#include <QCameraInfo>
#include <QCameraViewfinder>

#include "v4l2_service.h"

#define FILE_VIDEO "/dev/video0"

class CameraTest : public QObject
{
    Q_OBJECT
public:
    explicit CameraTest(QObject *parent = nullptr);

    void initWindow(int winWidth, int winHeight);

    CommonComponent componet;

    QTimer *timer;

    QWidget *widget;

    bool result;

    QLabel *lable;

public slots:
    void disply();
    void getTestResult(bool result);

signals:
    void update_btn_style();

};

#endif // CAMERATEST_H
