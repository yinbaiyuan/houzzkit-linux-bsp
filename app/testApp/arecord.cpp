#include "arecord.h"
#include "QAudioFormat"
#include "commoncomponent.h"
#include "QDebug"
#include "QFile"
#include "QAudioDeviceInfo"
#include "QAudioInput"
#include "QAudioOutput"
#include "QMediaPlayer"
#include <QTimer>

static CommonComponent componet;
static QPushButton *recordBtn;
static QLCDNumber *recordTime;
static QPushButton *playBtn;
static QLCDNumber *playTime;
static QFile outputFile;
static QAudioInput *input = nullptr;
static QAudioOutput *output = nullptr;
static QWidget *widget;
static QTimer *timer;
static QTimer *playTimeCutDown;
static int timeCount = 1;

#define SAMPLE_RATE 44100
#define CHANNEL_CCOUNT 2
#define SAMPLE_SIZE 16
#define AUDIO_CODEC "audio/pcm"
#define AUDIO_PATH "/userdata/record.wav"

ARecord::ARecord(QObject *parent) : QObject(parent)
{
//    initWindow();
}

void ARecord::initWindow(int screenWidth, int screenHeight)
{
    //初始化界面
    recordBtn = componet.getButton("录音");
    recordTime = componet.getLcdNumber();
    recordTime->display("00");
    playBtn = componet.getButton("播放");
    playTime = componet.getLcdNumber();
    playTime->display("00");
    widget = componet.getWidget();

    // ::system("amixer -c1 sset 'Playback Path' HP");

    connect(recordBtn, &QPushButton::clicked, [=] { recordAudio(); });
    connect(playBtn, &QPushButton::clicked, [=] { playAudio(); });

    QGridLayout *layout = new QGridLayout;
    QPushButton *tips = componet.getButton("自动录音三秒后自动播放，请注意听喇叭和耳机是否有音频输出！");
    tips->setStyleSheet(tips->styleSheet()+"QPushButton{background:none;color:black}QPushButton:pressed{background:none;}");

    layout->addWidget(tips, 1, 1, 1, 2);
    layout->addWidget(recordBtn, 2, 1, 1, 1);
    layout->addWidget(recordTime, 2, 2, 1, 1);
    layout->addWidget(playBtn, 3, 1, 1, 1);
    layout->addWidget(playTime, 3, 2, 1, 1);

    QPushButton *pass = componet.getButton("测试成功");
    QPushButton *faild = componet.getButton("测试失败");
    faild->setStyleSheet(faild->styleSheet()+"QPushButton{background-color:#f56c6c}");
    connect(pass, &QPushButton::clicked, [=] { this->getTestResult(true); });
    connect(faild, &QPushButton::clicked, [=] { this->getTestResult(false); });

    layout->addWidget(pass, 4, 1, 1, 1);
    layout->addWidget(faild, 4, 2, 1, 1);

    widget->setLayout(layout);

    timer = new QTimer(widget);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, [=] { this->autoPlay(); });

    playTimeCutDown = new QTimer(widget);
    playTimeCutDown->setInterval(1000);
    connect(playTimeCutDown, &QTimer::timeout, [=] { this->playTimeDown(); });

    widget->resize(screenWidth,screenHeight);
    widget->showFullScreen();

    emit recordBtn->clicked();
}

void ARecord::recordAudio()
{
    qDebug()<<"recordAudio";
    QAudioDeviceInfo info;

    if(recordBtn->text() == "录音"){
        recordBtn->setText("停止录音");
        outputFile.setFileName(AUDIO_PATH);
        outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
        recordTime->display("00");

        QAudioFormat format;
        format.setSampleRate(SAMPLE_RATE);
        format.setChannelCount(CHANNEL_CCOUNT);
        format.setSampleSize(SAMPLE_SIZE);
        format.setCodec(AUDIO_CODEC);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        QList<QAudioDeviceInfo> availableDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

        for (QAudioDeviceInfo device:availableDevices){
            qDebug()<<device.deviceName()<<"--support code:"<<device.supportedCodecs();
            if (device.isFormatSupported(format)){
                qDebug()<<"befor new";
//                if(input == nullptr)
                    input = new QAudioInput(device, format, widget);
                qDebug()<<"end new";
                info = device;
                break;
            }else{
                qDebug()<<device.deviceName()
                       <<"--not soupprot formatinfo:SampleRate="<<format.sampleRate()
                      <<","<<"ChannelCount="<<format.channelCount()
                     <<","<<"SampleSize="<<format.sampleSize()
                    <<","<<"Codec="<<format.codec();
            }
        }
        if (input){
            qDebug()<<"use device:"<<info.deviceName()<<endl;
            //TODO 开始计时
            input->reset();
            input->start(&outputFile);
            timeCount = 1;
            timer->start();
            qDebug()<<" timer->start";
        }

    }else{
        input->stop();
        outputFile.close();
//        delete input;
        recordBtn->setText("录音");
    }
}

void ARecord::playAudio()
{
    if(playBtn->text() != "播放"){
        playBtn->setText("播放");
        finishedPlaying(output->state());
    }else{
        playBtn->setText("停止播放");
        qDebug()<<"playAudio";
        outputFile.setFileName(AUDIO_PATH);
        outputFile.open( QIODevice::ReadOnly);
        playTime->display("0" + QString::number(timeCount));

        QAudioFormat format;
        format.setSampleRate(SAMPLE_RATE);
        format.setChannelCount(CHANNEL_CCOUNT);
        format.setSampleSize(SAMPLE_SIZE);
        format.setCodec(AUDIO_CODEC);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);
        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

        QList<QAudioDeviceInfo> availableDevices_o = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        for (QAudioDeviceInfo device:availableDevices_o){
            qDebug()<<device.deviceName()<<"--support code:"<<device.supportedCodecs();

            if (device.isFormatSupported(format)){
                output = new QAudioOutput(device, format, widget);
                info = device;
                break;
            }else{
                qDebug()<<device.deviceName()
                       <<"--not soupprot formatinfo:SampleRate="<<format.sampleRate()
                      <<","<<"ChannelCount="<<format.channelCount()
                     <<","<<"SampleSize="<<format.sampleSize()
                    <<","<<"Codec="<<format.codec();
            }
        }

        if(output){
            qDebug()<<"use device:"<<info.deviceName()<<endl;
            playTimeCutDown->start();
            output->start(&outputFile);
            connect(output, &QAudioOutput::stateChanged, [=] { finishedPlaying(output->state()); });
        }
    }
}

void ARecord::finishedPlaying(QAudio::State state)
{
    playBtn->setText("播放");
    if(state == QAudio::IdleState){
        output->stop();
        outputFile.close();
//        delete output;
    }
}

void ARecord::autoPlay()
{
    recordTime->display("0" + QString::number(timeCount));
    timeCount ++;
    if(timeCount > 3){
        timeCount = 3;
        input->stop();
        outputFile.close();
//        delete input;
        recordBtn->setText("录音");
        timer->stop();
        emit playBtn->clicked();
    }
}

void ARecord::playTimeDown()
{
    playTime->display("0" + QString::number(timeCount));
    timeCount--;
    if(timeCount < 0){
        timeCount = 0;
        playTimeCutDown->stop();
    }
}

void ARecord::getTestResult(bool testResult)
{
    result = testResult;
    emit update_btn_style();
    widget->close();
}


//如果直接关掉窗口的话，需要在析构函数中删除掉实例
ARecord::~ARecord(){
    if(input)
        delete input;

    if(output)
        delete output;
}
