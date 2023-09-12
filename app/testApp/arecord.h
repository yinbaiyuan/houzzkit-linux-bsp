#ifndef ARECORD_H
#define ARECORD_H

#include <QObject>
#include <QAudio>
class ARecord : public QObject
{
    Q_OBJECT
public:
    explicit ARecord(QObject *parent = nullptr);
    void initWindow(int screenWidth, int screenHeight);
    bool result;
    ~ ARecord();

public slots:
    void recordAudio();
    void playAudio();
    void finishedPlaying(QAudio::State state);
    void autoPlay();
    void getTestResult(bool result);
    void playTimeDown();

signals:
    void update_btn_style();

};

#endif // ARECORD_H
