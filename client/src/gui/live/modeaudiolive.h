#ifndef MODEAUDIOLIVE_H
#define MODEAUDIOLIVE_H

#include <QVBoxLayout>
//#include <QScrollArea>
#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QMessageBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QLCDNumber>
#include <QDateTime>
#include <QDesktopWidget>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QAudioRecorder>
//#include <QMediaRecorder>
#include <iostream>
#include <thread>
//#include <mutex>
#include <string>
#include <cmath>
#include <map>
#include <list>
#include <qendian.h>
#include "Systemcall.hpp"
#include "live/modelive.h"

//#define PCM_CHANNEL_COUNT 1
#define PCM_SAMPLE_SIZE 16
#define BUFFER_SIZE 1024
#define UPDATE_INTERVAL_MS 20 // 20 => 0,02sec => 50 time per seconde

class modeAudioLive : public QObject
{
    Q_OBJECT

enum AudioStatus {
    STARTING,
    RUNNING,
    STOPPING,
    STOPPED
};

public:
    modeAudioLive(QWidget *live_audio_tab, modeLive *mode_live);
    ~modeAudioLive();

    void startAudio(QAudioDeviceInfo inputDevice);
    void stopAudio();
    //AudioStatus getStatus() {return this->status;};

public slots:
    void updateOutputSelect();

private:
    void setUpAudio(QAudioDeviceInfo inputDevice);
    void useCheckedOutput();
    void updateAudioGui();
    void changeStatus(AudioStatus status, QString added_text = "");
    void resetTelemetry();
    void setMaxAplitude();
    void audioBufferToLevel(char *data, int len);

private slots:
    void updateAudioValue(int value);
    void buffer_processing();

signals:
    void current_value_update(int value);
    //void current_value_update(char *data, int len);

private:
    QWidget *live_audio_tab;
    modeLive *mode_live;
    QAudioInput *audioInput;
    QIODevice *audioDevice;
    QAudioFormat formatAudio;
    quint32 format_maxAmplitude;

    AudioStatus status;

    std::thread *buffer_process_thread;
    bool buffer_thread_running;

    /* value & telemetry */
    int current_value;

    int max_value;
    int min_value;

    //std::map<QString, int> live_output_list;
    std::map<int, QCheckBox*> live_output_list;
    //std::map<QString, int> checked_live_output;

    //QVBoxLayout *out_vlayout;
    //std::map<QString, dynamicOutput*> dyn_o_widgets;
};

#endif // MODEAUDIOLIVE_H
