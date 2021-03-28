#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QStyle>
#include <QDesktopWidget>
#include "../tools/serverApi.hpp"
#include "../tools/settingsmanager.hpp"

namespace Ui {
class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(serverApi *server_api, SettingsManager *settings_manager, QWidget *parent = nullptr);
    ~SettingsPage();

signals:
    /* LIVE */
    void live_input_poll_time_changed(int value);

    /* LIVE AUDIO */
    void live_audio_update_interval_changed(int value);
    void live_audio_normalize_timeout_changed(int value);
    void live_audio_normalize_min_value_changed(int value);
    void live_audio_linear_change_rate_changed(double value);
    void live_audio_max_value_change_rate_changed(double value);
    void live_audio_max_value_change_cooldown_changed(int value);

private slots:
    /* GENERAL */
    void setServerStayAlive(bool value);
    void on_general_server_stay_active_checkBox_toggled(bool checked);

    /* LIVE */
    void on_live_input_poll_time_spinBox_valueChanged(int arg1);
    void on_live_reset_button_clicked();

    /* LIVE AUDIO */
    void on_live_audio_update_interval_spinBox_valueChanged(int arg1);
    void on_live_audio_normalize_timeout_spinBox_valueChanged(int arg1);
    void on_live_audio_normalize_min_value_spinBox_valueChanged(int arg1);
    void on_live_audio_linear_change_rate_doubleSpinBox_valueChanged(double arg1);
    void on_live_audio_max_value_change_rate_doubleSpinBox_valueChanged(double arg1);
    void on_live_audio_max_value_change_cooldown_spinBox_valueChanged(int arg1);
    void on_live_audio_reset_button_clicked();

private:
    Ui::SettingsPage *ui;
    serverApi *server_api;
    SettingsManager *settings_manager;
};

#endif // SETTINGSPAGE_H
