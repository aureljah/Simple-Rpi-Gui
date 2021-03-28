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
#include "../tools/settingsmanager.hpp"

//#define PCM_CHANNEL_COUNT 1
#define PCM_SAMPLE_SIZE 16
#define BUFFER_SIZE 1024

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
    modeAudioLive(QWidget *live_audio_tab, modeLive *mode_live, serverApi *server_api, SettingsManager *setting_manager);
    ~modeAudioLive();

    void startAudio(QAudioDeviceInfo inputDevice);
    void stopAudio();
    AudioStatus getStatus() {return this->status;};

    /* options */
    void setGain(int gain);
    void setMultiplier(double mult);
    void setUseLinearChange(bool checked);
    void setUseMaxValueChange(bool checked);
    void setMaxValueChange(int value);
    void setUseNormalize(bool checked);
    void resetTelemetry();

public slots:
    void updateOutputSelect();

    /* settings */
    void setUpdateInterval(int interval);
    void setNormalizeTimeout(int timeout);
    void setNormalizeMinValue(int min_value);
    void setLinearChangeRate(double change_rate);
    void setMaxValueChangeRate(double change_rate);
    void setMaxValueChangeCooldown(int cooldown);

private:
    bool setUpAudio(QAudioDeviceInfo inputDevice);
    void useCheckedOutput();
    void updateAudioGui();
    void changeStatus(AudioStatus status, QString added_text = "");
    void setMaxAmplitude();
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
    SettingsManager *setting_manager;

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
    int update_interval_ms;
    int normalize_timeout_ms;
    int normalize_min_value;
    double linear_change_rate;
    double max_value_change_rate;
    int max_value_change_cooldown;

    /* options */
    int gain;
    double multiplier;
    int normalize_max_level;
    qint64 normalize_last_time_max_reched;

    bool use_normalize;
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
