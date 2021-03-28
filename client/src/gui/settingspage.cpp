#include "settingspage.h"
#include "ui_settingspage.h"

SettingsPage::SettingsPage(serverApi *server_api, SettingsManager *settings_manager, QWidget *parent)
    : QWidget(parent, Qt::Window), ui(new Ui::SettingsPage),
      server_api(server_api), settings_manager(settings_manager)
{
    this->move(parent->window()->frameGeometry().topLeft() + this->rect().center());

    ui->setupUi(this);
    setWindowTitle("Settings");

    QObject::connect(this->server_api, &serverApi::stay_alive_setting,
                     this, &SettingsPage::setServerStayAlive);
    this->server_api->getServerSetting();

    ui->live_audio_update_interval_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::SETTING_AUDIO_UPDATE_INTERVAL));
    ui->live_audio_normalize_timeout_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::SETTING_AUDIO_NORMALIZE_TIMEOUT));
    ui->live_audio_normalize_min_value_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::SETTING_AUDIO_NORMALIZE_MIN_VALUE));
    ui->live_audio_linear_change_rate_doubleSpinBox->setValue(this->settings_manager->getDataDouble(SettingsManager::SETTING_AUDIO_LINEAR_CHANGE_RATE));
    ui->live_audio_max_value_change_rate_doubleSpinBox->setValue(this->settings_manager->getDataDouble(SettingsManager::SETTING_AUDIO_MAX_VALUE_CHANGE_RATE));
    ui->live_audio_max_value_change_cooldown_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN));
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::setServerStayAlive(bool value)
{
    ui->general_server_stay_active_checkBox->setChecked(value);
}

void SettingsPage::on_general_server_stay_active_checkBox_toggled(bool checked)
{
    if (checked == true)
        this->server_api->setServerSetting("stay_active", "true");
    else
        this->server_api->setServerSetting("stay_active", "false");
}

void SettingsPage::on_live_input_poll_time_spinBox_valueChanged(int arg1)
{
    emit live_input_poll_time_changed(arg1);
}
void SettingsPage::on_live_reset_button_clicked()
{
    ui->live_input_poll_time_spinBox->setValue(this->settings_manager->getDefaultIntValue(SettingsManager::SETTING_LIVE_INPUT_POLL_TIME));
}

void SettingsPage::on_live_audio_update_interval_spinBox_valueChanged(int arg1)
{
    emit live_audio_update_interval_changed(arg1);
}
void SettingsPage::on_live_audio_normalize_timeout_spinBox_valueChanged(int arg1)
{
    emit live_audio_normalize_timeout_changed(arg1);
}
void SettingsPage::on_live_audio_normalize_min_value_spinBox_valueChanged(int arg1)
{
    emit live_audio_normalize_min_value_changed(arg1);
}
void SettingsPage::on_live_audio_linear_change_rate_doubleSpinBox_valueChanged(double arg1)
{
    emit live_audio_linear_change_rate_changed(arg1);
}
void SettingsPage::on_live_audio_max_value_change_rate_doubleSpinBox_valueChanged(double arg1)
{
    emit live_audio_max_value_change_rate_changed(arg1);
}
void SettingsPage::on_live_audio_max_value_change_cooldown_spinBox_valueChanged(int arg1)
{
    emit live_audio_max_value_change_cooldown_changed(arg1);
}
void SettingsPage::on_live_audio_reset_button_clicked()
{
    ui->live_audio_update_interval_spinBox->setValue(this->settings_manager->getDefaultIntValue(SettingsManager::SETTING_AUDIO_UPDATE_INTERVAL));
    ui->live_audio_normalize_timeout_spinBox->setValue(this->settings_manager->getDefaultIntValue(SettingsManager::SETTING_AUDIO_NORMALIZE_TIMEOUT));
    ui->live_audio_normalize_min_value_spinBox->setValue(this->settings_manager->getDefaultIntValue(SettingsManager::SETTING_AUDIO_NORMALIZE_MIN_VALUE));
    ui->live_audio_linear_change_rate_doubleSpinBox->setValue(this->settings_manager->getDefaultDoubleValue(SettingsManager::SETTING_AUDIO_LINEAR_CHANGE_RATE));
    ui->live_audio_max_value_change_rate_doubleSpinBox->setValue(this->settings_manager->getDefaultDoubleValue(SettingsManager::SETTING_AUDIO_MAX_VALUE_CHANGE_RATE));
    ui->live_audio_max_value_change_cooldown_spinBox->setValue(this->settings_manager->getDefaultIntValue(SettingsManager::SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN));
}
