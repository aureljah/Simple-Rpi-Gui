#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVBoxLayout>
#include <QMainWindow>
#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
//#include <thread>
#include <mutex>
#include <map>
#include "Socket.hpp" // OS specific before other !
#include "../tools/serverApi.hpp"
#include "../tools/settingsmanager.hpp"
#include "connection.h"
#include "settingspage.h"
#include "live/modelive.h"
#include "live/modeaudiolive.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startConnectWin();
    void writeToDebugScreen(std::string msg, std::string type);
    void pingServeur(QString msg);

    void setFadeIn(bool value);
    void setFadeOut(bool value);

signals:
    void new_serv_msg(QString msg);
    void enter_audio_tab();

private slots:
    void onConnected();
    void on_pushButton_clicked();
    void onMsgFromServer(QString msg);

    void error_500_handler(std::string fct_name, std::string msg);

    void on_live_add_output_pushButton_clicked();
    void on_live_add_input_pushButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_live_audio_controller_startButton_clicked();
    void on_live_audio_controller_stopButton_clicked();
    void on_live_audio_gain_spinBox_valueChanged(int arg1);
    void on_live_audio_mult_doubleSpinBox_valueChanged(double arg1);
    void on_live_audio_setting_reset_button_clicked();
    void on_live_audio_linear_change_checkBox_toggled(bool checked);
    void on_live_audio_max_value_change_checkBox_toggled(bool checked);
    void on_live_audio_max_value_change_spinBox_valueChanged(int arg1);
    void on_audio_reset_telemetry_button_clicked();
    void on_live_audio_normalize_checkBox_toggled(bool checked);

    void on_settings_action_triggered();
    void on_actionLive_output_use_fade_In_effect_toggled(bool arg1);
    void on_actionLive_output_use_fade_Out_effect_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    ISocket *main_sock;
    serverApi *server_api;
    SettingsManager *settings_manager;

    modeLive *mode_live;
    modeAudioLive *mode_audio_live;

    //std::thread *t2;
    std::mutex lock_text_screen;

};

#endif // MAINWINDOW_H
