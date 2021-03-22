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
#include <vector>
#include <list>
#include <qendian.h>
#include "Systemcall.hpp"
#include "live/modelive.h"

//#define PCM_CHANNEL_COUNT 1
#define PCM_SAMPLE_SIZE 16
#define BUFFER_SIZE 2048
#define UPDATE_INTERVAL_MS 50 // 20 => 0,02sec => 50 time per seconde

#define LINEAR_CHANGE_RATE 0.02
#define MAX_VALUE_CHANGE_RATE 0.5
#define MAX_VALUE_CHANGE_RATE_COOLDOWN_MS 250

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
    modeAudioLive(QWidget *live_audio_tab, modeLive *mode_live, serverApi *server_api);
    ~modeAudioLive();

    void startAudio(QAudioDeviceInfo inputDevice);
    void stopAudio();
    AudioStatus getStatus() {return this->status;};

    void setGain(int gain);
    void setMultiplier(double mult);
    void setUseLinearChange(bool checked);
    void setUseMaxValueChange(bool checked);
    void setMaxValueChange(int value);
    void resetTelemetry();

public slots:
    void updateOutputSelect();

private:
    bool setUpAudio(QAudioDeviceInfo inputDevice);
    void useCheckedOutput();
    void updateAudioGui();
    void changeStatus(AudioStatus status, QString added_text = "");
    void setMaxAplitude();
    void audioBufferToLevel(char *data, int len);

    bool checkOutputCoef(std::vector<int> output_list);
    void updateLinearChangeCoef();
    void updateMaxValueChange();

    void printSelectedFormat();

private slots:
    void updateAudioValue(int raw_value);
    void buffer_processing();

    void outputSelectedChanged();

signals:
    void current_value_update(int value);
    //void current_value_update(char *data, int len);

private:
    QWidget *live_audio_tab;
    modeLive *mode_live;
    serverApi *server_api;

    QAudioInput *audioInput;
    QIODevice *audioDevice;
    QAudioFormat formatAudio;
    quint32 format_maxAmplitude;

    AudioStatus status;

    std::thread *buffer_process_thread;
    bool buffer_thread_running;

    /* value & telemetry */
    int current_value;
    int raw_value;

    int max_value;
    int min_value;

    /* settings */
    int gain;
    double multiplier;

    bool use_linear_change;
    bool use_max_value_change;
    int max_value_change;
    qint64 last_time_max_change;
    // coef for each output (used by linear and max value change)
    std::map<int, float> output_coef;
    int main_output_linear_pin;

    std::map<int, QCheckBox*> live_output_list;
};

#endif // MODEAUDIOLIVE_H
