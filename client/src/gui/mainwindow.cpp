#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), mode_audio_live(nullptr)
{
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->screenGeometry(this)));
    ui->setupUi(this);

    this->main_sock = new Socket("mycert.pem", "key.pem", OpensslWrapper::CLIENT);
    this->server_api = new serverApi(this->main_sock);
    QObject::connect(this->server_api, &serverApi::send_recv_server_msg,
                     this, &MainWindow::writeToDebugScreen);
    QObject::connect(this->server_api, &serverApi::use_fade_in_setting,
                     this, &MainWindow::setFadeIn);
    QObject::connect(this->server_api, &serverApi::use_fade_out_setting,
                     this, &MainWindow::setFadeOut);
    QObject::connect(this->server_api, &serverApi::error_500,
                     this, &MainWindow::error_500_handler);

    this->settings_manager = new SettingsManager();

    this->mode_live = new modeLive(ui->live_tab, this->server_api, this->settings_manager);
    QObject::connect(this->server_api, &serverApi::new_live_input,
                     this->mode_live, &modeLive::add_input);
    QObject::connect(this->server_api, &serverApi::new_live_output,
                     this->mode_live, &modeLive::add_output);
    QObject::connect(this->server_api, &serverApi::input_value_changed,
                     this->mode_live, &modeLive::update_input_value);

    this->startConnectWin();

    this->mode_audio_live = new modeAudioLive(ui->audio_tab, this->mode_live, this->server_api, this->settings_manager);
    QObject::connect(this, &MainWindow::enter_audio_tab,
                     this->mode_audio_live, &modeAudioLive::updateOutputSelect);

    auto info = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    ui->live_audio_controller_comboBox->clear();
    foreach (auto i, info) ui->live_audio_controller_comboBox->addItem(i.deviceName());
    ui->live_audio_controller_comboBox->setCurrentText(QAudioDeviceInfo::defaultInputDevice().deviceName());

    ui->live_audio_controller_comboBox->setCurrentText(this->settings_manager->getDataStr(SettingsManager::AUDIO_LAST_AUDIOINPUT));
    ui->live_audio_gain_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::AUDIO_LAST_GAIN));
    ui->live_audio_mult_doubleSpinBox->setValue(this->settings_manager->getDataDouble(SettingsManager::AUDIO_LAST_MULTIPLIER));
    ui->live_audio_normalize_checkBox->setChecked(this->settings_manager->getDataBool(SettingsManager::AUDIO_LAST_USE_NORMALIZE));
    ui->live_audio_linear_change_checkBox->setChecked(this->settings_manager->getDataBool(SettingsManager::AUDIO_LAST_USE_LINEAR));
    ui->live_audio_max_value_change_checkBox->setChecked(this->settings_manager->getDataBool(SettingsManager::AUDIO_LAST_USE_MAX_VALUE_CHANGE));
    ui->live_audio_max_value_change_spinBox->setValue(this->settings_manager->getDataInt(SettingsManager::AUDIO_LAST_MAX_VALUE_CHANGE));
}

MainWindow::~MainWindow()
{
    delete this->main_sock;
    delete this->ui;
}

void MainWindow::startConnectWin()
{
    auto win = new connection(this->main_sock, this->settings_manager);

    QObject::connect(win, &connection::connected,
                     this, &MainWindow::onConnected);

    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void MainWindow::writeToDebugScreen(std::string msg, std::string type)
{
    QString full_msg = "";
    if (type == "send")
    {
        full_msg = "[CLIENT]: " + QString::fromStdString(msg);
    }
    else if (type == "recv")
    {
        full_msg = "[SERVER]: " + QString::fromStdString(msg);
    }
    else
        return;

    lock_text_screen.lock();
    ui->plainTextEdit->appendPlainText(full_msg);
    lock_text_screen.unlock();
}

void MainWindow::pingServeur(QString msg)
{
    lock_text_screen.lock();
    this->main_sock->write(msg.toStdString());
    ui->plainTextEdit->appendPlainText("[CLIENT]: " + msg);
    std::string serv_msg = this->main_sock->read(4096);
    ui->plainTextEdit->appendPlainText("[SERVER]: " + QString::fromStdString((serv_msg)));
    lock_text_screen.unlock();
    //qInfo() << "[CLIENT] server: " << QString::fromStdString(msg) << "\n";
}

void MainWindow::onMsgFromServer(QString msg)
{
    ui->plainTextEdit->appendPlainText("[SERVER]: " + msg);
}

void MainWindow::onConnected()
{
    this->show();
    QObject::connect(this, &MainWindow::new_serv_msg,
                     this, &MainWindow::onMsgFromServer);

    this->server_api->getServerStatus();
    this->server_api->getServerSetting();

    this->mode_audio_live->updateOutputSelect();
}

void MainWindow::error_500_handler(std::string fct_name, std::string msg)
{
    qInfo() << "error_500_handler: received error 500 from serverApi\n";

    // do things due to error
    this->mode_audio_live->stopAudio();

    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString(fct_name) + " Error 500: " + QString::fromStdString(msg));
    msgBox.exec();
}

/* send ping/msg to server boutton */
void MainWindow::on_pushButton_clicked()
{
    qInfo() << "[DEBUG]: Debug input: " << ui->lineEdit->text() << "\n";
    this->pingServeur(ui->lineEdit->text());
    ui->lineEdit->clear();
}

/* add output boutton */
void MainWindow::on_live_add_output_pushButton_clicked()
{
    this->mode_live->startGpioSettingDialog(gpioSettingDialog::OUTPUT);
}

/* add output boutton */
void MainWindow::on_live_add_input_pushButton_clicked()
{
    this->mode_live->startGpioSettingDialog(gpioSettingDialog::INPUT);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (ui->tabWidget->currentWidget() == ui->audio_tab)
    {
        emit enter_audio_tab();

        /*auto audioRecorder = new QAudioRecorder();
        const QStringList inputs = audioRecorder->audioInputs();
        QString selectedInput = audioRecorder->defaultAudioInput();

        for (const QString &input : inputs) {
            QString description = audioRecorder->audioInputDescription(input);
            qInfo() << input << ": " << description << "\n";
            selectedInput = input;
        }*/
    }
}

void MainWindow::on_live_audio_controller_startButton_clicked()
{
    auto info = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    QAudioDeviceInfo selected_device = QAudioDeviceInfo::defaultInputDevice();
    foreach (auto i, info)
    {
        if (i.deviceName() == ui->live_audio_controller_comboBox->currentText())
        {
            selected_device = i;
            break;
        }
    }
    this->mode_audio_live->startAudio(selected_device);
}

void MainWindow::on_live_audio_controller_stopButton_clicked()
{
    this->mode_audio_live->stopAudio();
}

void MainWindow::on_live_audio_gain_spinBox_valueChanged(int arg1)
{
    this->mode_audio_live->setGain(arg1);
}

void MainWindow::on_live_audio_mult_doubleSpinBox_valueChanged(double arg1)
{
    this->mode_audio_live->setMultiplier(arg1);
}

void MainWindow::on_live_audio_setting_reset_button_clicked()
{
    ui->live_audio_gain_spinBox->setValue(0);
    ui->live_audio_mult_doubleSpinBox->setValue(1);
    ui->live_audio_max_value_change_spinBox->setValue(80);
    ui->live_audio_linear_change_checkBox->setChecked(false);
    ui->live_audio_max_value_change_checkBox->setChecked(false);
    ui->live_audio_normalize_checkBox->setChecked(false);
}

void MainWindow::on_live_audio_linear_change_checkBox_toggled(bool checked)
{
    this->mode_audio_live->setUseLinearChange(checked);
}

void MainWindow::on_live_audio_max_value_change_checkBox_toggled(bool checked)
{
    this->mode_audio_live->setUseMaxValueChange(checked);
}

void MainWindow::on_live_audio_max_value_change_spinBox_valueChanged(int arg1)
{
    this->mode_audio_live->setMaxValueChange(arg1);
}

void MainWindow::on_audio_reset_telemetry_button_clicked()
{
    this->mode_audio_live->resetTelemetry();
}

void MainWindow::on_live_audio_normalize_checkBox_toggled(bool checked)
{
    this->mode_audio_live->setUseNormalize(checked);
}

void MainWindow::on_settings_action_triggered()
{
    auto win = new SettingsPage(this->server_api, this->settings_manager, this);

    QObject::connect(win, &SettingsPage::live_input_poll_time_changed,
                     this->mode_live, &modeLive::setInputPollTime);
    QObject::connect(win, &SettingsPage::live_audio_update_interval_changed,
                     this->mode_audio_live, &modeAudioLive::setUpdateInterval);
    QObject::connect(win, &SettingsPage::live_audio_normalize_timeout_changed,
                     this->mode_audio_live, &modeAudioLive::setNormalizeTimeout);
    QObject::connect(win, &SettingsPage::live_audio_normalize_min_value_changed,
                     this->mode_audio_live, &modeAudioLive::setNormalizeMinValue);
    QObject::connect(win, &SettingsPage::live_audio_linear_change_rate_changed,
                     this->mode_audio_live, &modeAudioLive::setLinearChangeRate);
    QObject::connect(win, &SettingsPage::live_audio_max_value_change_rate_changed,
                     this->mode_audio_live, &modeAudioLive::setMaxValueChangeRate);
    QObject::connect(win, &SettingsPage::live_audio_max_value_change_cooldown_changed,
                     this->mode_audio_live, &modeAudioLive::setMaxValueChangeCooldown);

    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void MainWindow::setFadeIn(bool value)
{
    ui->actionLive_output_use_fade_In_effect->setChecked(value);
}
void MainWindow::on_actionLive_output_use_fade_In_effect_toggled(bool arg1)
{
    if (arg1 == true)
        this->server_api->setServerSetting("use_fade_in", "true");
    else
        this->server_api->setServerSetting("use_fade_in", "false");
}
void MainWindow::setFadeOut(bool value)
{
    ui->actionLive_output_use_fade_Out_effect->setChecked(value);
}
void MainWindow::on_actionLive_output_use_fade_Out_effect_toggled(bool arg1)
{
    if (arg1 == true)
        this->server_api->setServerSetting("use_fade_out", "true");
    else
        this->server_api->setServerSetting("use_fade_out", "false");
}
