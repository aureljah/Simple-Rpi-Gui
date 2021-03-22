#include "modeaudiolive.h"

modeAudioLive::modeAudioLive(QWidget *live_audio_tab, modeLive *mode_live, serverApi *server_api)
    : live_audio_tab(live_audio_tab), mode_live(mode_live), server_api(server_api), audioInput(nullptr), buffer_process_thread(nullptr),
    current_value(0), max_value(0), min_value(-1), gain(0), multiplier(1),
    use_linear_change(false), use_max_value_change(false), max_value_change(80), last_time_max_change(0), main_output_linear_pin(-1)
{
    this->changeStatus(AudioStatus::STOPPED);
    this->updateAudioGui();

    // TODO ONE TIME
    QScrollArea *output_scrollArea = live_audio_tab->findChild<QScrollArea*>("live_audio_select_scrollArea", Qt::FindChildrenRecursively);
    if (output_scrollArea != nullptr) {
        QVBoxLayout *layout = new QVBoxLayout();
        output_scrollArea->setLayout(layout);
        output_scrollArea->setWidgetResizable(true);
    }
}

modeAudioLive::~modeAudioLive()
{

}

void modeAudioLive::setGain(int gain)
{
    if (gain >= -99 && gain <= 99)
        this->gain = gain;
}
void modeAudioLive::setMultiplier(double mult)
{
    if (mult > 0 && mult <= 10)
        this->multiplier = mult;
}
void modeAudioLive::setUseLinearChange(bool checked)
{
    this->use_linear_change = checked;

    // reset linear & max change
    //this->main_output_linear_pin = -1;
    //this->output_coef.clear();
}
void modeAudioLive::setUseMaxValueChange(bool checked)
{
    this->use_max_value_change = checked;

    // reset linear & max change
    //this->main_output_linear_pin = -1;
    //this->output_coef.clear();
}
void modeAudioLive::setMaxValueChange(int value)
{
    if (value >= 1 && value <= 101)
        this->max_value_change = value;
}

void modeAudioLive::startAudio(QAudioDeviceInfo inputDevice)
{
    if (this->status != AudioStatus::STOPPED) {
        qInfo() << "Cannot start live audio when is not in stopped state!\n";
        return;
    }

    this->changeStatus(AudioStatus::STARTING);

    if (inputDevice.isNull()) {
        QMessageBox::warning(nullptr, "Live Audio",
                             "There is no audio input device available.");
        this->changeStatus(AudioStatus::STOPPED, "ERROR NO INPUT");
        return;
    }

    this->resetTelemetry();

    if (this->setUpAudio(inputDevice) == false)
        this->changeStatus(AudioStatus::STOPPED, "FORMAT NOT SUPPORTED");
    else
        this->changeStatus(AudioStatus::RUNNING);
}

void modeAudioLive::stopAudio()
{
    this->changeStatus(AudioStatus::STOPPING);
    this->buffer_thread_running = false;
    if (this->buffer_process_thread)
    {
        this->buffer_process_thread->join();
        delete this->buffer_process_thread;
        this->buffer_process_thread = nullptr;
    }
    if (this->audioInput) {
        QAudioInput *audio_input = this->audioInput;
        this->audioInput = nullptr;
        audio_input->stop();
        delete audio_input;
    }
    this->resetTelemetry();
    this->output_coef.clear();
    this->main_output_linear_pin = -1;
    this->changeStatus(AudioStatus::STOPPED);
}

void modeAudioLive::updateOutputSelect()
{
    QScrollArea *output_scrollArea = live_audio_tab->findChild<QScrollArea*>("live_audio_select_scrollArea", Qt::FindChildrenRecursively);
    if (output_scrollArea != nullptr)
    {
        // add and update title
        std::map<QString, int> new_output_list = this->mode_live->getOutputNamePinList();
        for(std::map<QString, int>::iterator it = new_output_list.begin(); it != new_output_list.end(); it++) {
            if (this->live_output_list.find(it->second) != this->live_output_list.end()) {
                QCheckBox *check = this->live_output_list[it->second];
                if (check->text() != it->first) {
                    check->setText(it->first);
                }
            }
            else {
                QCheckBox *check = new QCheckBox();
                this->live_output_list[it->second] = check;
                check->setText(it->first);
                output_scrollArea->layout()->addWidget(check);
            }
        }
        //qInfo() << "updateOutputSelect: finsihed add and update title: live_output_list.size: " << this->live_output_list.size() << "\n";
        // remove old
        std::map<int, QCheckBox*>::iterator it = this->live_output_list.begin();
        while (it != this->live_output_list.end()) {
            bool found = false;

            for(std::map<QString, int>::iterator new_it = new_output_list.begin(); new_it != new_output_list.end(); new_it++) {
                if (it->first == new_it->second) {
                    found = true;
                    break;
                }
            }

            if (found == false) {
                QCheckBox *check = it->second;
                if (output_scrollArea->layout()) {
                    output_scrollArea->layout()->removeWidget(check);
                }
                check->deleteLater();
                it = this->live_output_list.erase(it);
            }
            else {
                it++;
            }
        }

    }
}

bool modeAudioLive::setUpAudio(QAudioDeviceInfo inputDevice)
{
    this->formatAudio.setSampleRate(8000);
    this->formatAudio.setChannelCount(1);
    this->formatAudio.setSampleSize(PCM_SAMPLE_SIZE);
    this->formatAudio.setCodec("audio/pcm");
    this->formatAudio.setByteOrder(QAudioFormat::LittleEndian);
    this->formatAudio.setSampleType(QAudioFormat::SignedInt);
    if (inputDevice.isFormatSupported(this->formatAudio) == false)
    {
        qWarning() << "modeAudioLive WARNING: Format not supported\n";
        this->formatAudio = inputDevice.nearestFormat(this->formatAudio);
        if (inputDevice.isFormatSupported(this->formatAudio) == false)
        {
            qWarning() << "modeAudioLive WARNING: Nearest format is also not supported\n";
            return false;
        }
    }
    this->printSelectedFormat();
    this->setMaxAplitude();

    this->audioInput = new QAudioInput(inputDevice, this->formatAudio, this);
    this->audioInput->setBufferSize(BUFFER_SIZE);

    this->audioDevice = this->audioInput->start();
    /*QObject::connect(this->audioDevice, &QIODevice::readyRead,
                     this, &modeAudioLive::buffer_processing);*/
    QObject::connect(this, &modeAudioLive::current_value_update,
                     this, &modeAudioLive::updateAudioValue);
    //connect(this->audioDevice, SIGNAL(readyRead()),
              //this, SLOT(buffer_processing()));
    this->audioDevice->open(QIODevice::ReadOnly);
    this->buffer_thread_running = true;
    this->buffer_process_thread = new std::thread(&modeAudioLive::buffer_processing, this);
    return true;
}

void modeAudioLive::printSelectedFormat()
{
    qInfo() << "Selected format sampleRate:" << this->formatAudio.sampleRate() << "\n";
    qInfo() << "Selected format sampleSize:" << this->formatAudio.sampleSize() << "\n";
    qInfo() << "Selected format sampleType:" << this->formatAudio.sampleType() << "\n";
    qInfo() << "Selected format channelCount:" << this->formatAudio.channelCount() << "\n";
    qInfo() << "Selected format byteOrder:" << this->formatAudio.byteOrder() << "\n";
    qInfo() << "Selected format codec:" << this->formatAudio.codec() << "\n";
}

void modeAudioLive::buffer_processing()
{
    qInfo() << "buffer_processing starting - isSequential: " << this->audioDevice->isSequential() << "\n";
    qInfo() << "buffer_processing starting - isReadable: " << this->audioDevice->isReadable() << "\n";
    //const int channelBytes = floor(PCM_SAMPLE_SIZE / 8);
    //const int sampleBytes = PCM_CHANNEL_COUNT * channelBytes;

    qint64 last_time = 0;
    char *buffer = new char[BUFFER_SIZE + 2];
    while (this->buffer_thread_running)
    {
        //qint64 bytesReady = this->audioInput->bytesReady();

        for (int i = 0 ; i < BUFFER_SIZE + 2; i++)
        {   buffer[i] = 0;  }

        qint64 readed = this->audioDevice->read(buffer, BUFFER_SIZE);
        if (readed > 0)
        {
            //qInfo() << "buffer_processing: readed: " << readed << " - bytesReady: " << bytesReady << "\n";

            //emit current_value_update(buffer, readed);
            qint64 cur_time = QDateTime::currentMSecsSinceEpoch();
            if (cur_time >= last_time + UPDATE_INTERVAL_MS) {
                last_time = cur_time;
                this->audioBufferToLevel(buffer, readed);
            }
        }
        else
            systemcall::sys_usleep(100);
    }
    delete[] buffer;
    qInfo() << "buffer_processing stopping\n";
}

void modeAudioLive::audioBufferToLevel(char *data, int len) {
    if (this->format_maxAmplitude) {
        Q_ASSERT(this->formatAudio.sampleSize() % 8 == 0);
        const int channelBytes = this->formatAudio.sampleSize() / 8;
        const int sampleBytes = this->formatAudio.channelCount() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        quint32 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i) {
            for (int j = 0; j < this->formatAudio.channelCount(); ++j) {
                quint32 value = 0;

                if (this->formatAudio.sampleSize() == 8 && this->formatAudio.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (this->formatAudio.sampleSize() == 8 && this->formatAudio.sampleType() == QAudioFormat::SignedInt) {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                } else if (this->formatAudio.sampleSize() == 16 && this->formatAudio.sampleType() == QAudioFormat::UnSignedInt) {
                    if (this->formatAudio.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (this->formatAudio.sampleSize() == 16 && this->formatAudio.sampleType() == QAudioFormat::SignedInt) {
                    if (this->formatAudio.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                } else if (this->formatAudio.sampleSize() == 32 && this->formatAudio.sampleType() == QAudioFormat::UnSignedInt) {
                    if (this->formatAudio.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                } else if (this->formatAudio.sampleSize() == 32 && this->formatAudio.sampleType() == QAudioFormat::SignedInt) {
                    if (this->formatAudio.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                } else if (this->formatAudio.sampleSize() == 32 && this->formatAudio.sampleType() == QAudioFormat::Float) {
                    value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        //qInfo() << "audioBufferToLevel: maxValue: " << maxValue << "\n";
        maxValue = qMin(maxValue, this->format_maxAmplitude);
        qreal level = qreal(maxValue) / this->format_maxAmplitude;
        //qInfo() << "audioBufferToLevel: level: " << level << "\n";

        emit current_value_update(floor(level * 100));
        //this->updateAudioValue(floor(level * 100));
    }
}

void modeAudioLive::updateAudioValue(int raw_value)
{
    int value = raw_value * this->multiplier;
    value += this->gain;
    if (value > 100)
        value = 100;
    if (value < 0)
        value = 0;

    qInfo() << "updateAudioValue: value: " << value << "raw value: " << raw_value << "\n";

    this->current_value = value;
    this->raw_value = raw_value;
    if (value < this->min_value || this->min_value == -1)
        this->min_value = value;
    if (value > this->max_value)
        this->max_value = value;

    this->updateAudioGui();

    this->useCheckedOutput();
}

void modeAudioLive::useCheckedOutput()
{
    std::vector<int> checked_pin_list;
    std::vector<int> output_pin_list;
    std::vector<int> output_value_list;
    std::vector<std::string> output_name_list;

    std::map<int, QCheckBox*>::iterator it = this->live_output_list.begin();
    while (it != this->live_output_list.end())
    {
        if (it->second->isChecked() == true)
            checked_pin_list.push_back(it->first);
        it++;
    }

    for(std::vector<int>::iterator it = checked_pin_list.begin(); it != checked_pin_list.end(); it++)
    {
        QString name = this->mode_live->getOutputNameFromPin(*it);
        if (name != "")
        {
            output_pin_list.push_back(*it);
            output_value_list.push_back(this->current_value);
            output_name_list.push_back(name.toStdString());
            this->mode_live->update_output_value(*it, this->current_value, true);
        }
    }
    if (output_pin_list.size() < 1 || (output_pin_list.size() != output_value_list.size()))
        return;

    if (this->checkOutputCoef(output_pin_list) == true)
    {
        if (this->use_linear_change == true)
            this->updateLinearChangeCoef();

        if (this->use_max_value_change == true)
            this->updateMaxValueChange();

        if (this->use_linear_change == true || this->use_max_value_change == true)
        {
            for(size_t i = 0 ; i < output_pin_list.size() ; i++)
            {
                output_value_list[i] = floor(this->current_value * this->output_coef[output_pin_list[i]]);
                qInfo() << "using output_coef: pin: " << output_pin_list[i] << "-  current_value: " << this->current_value << " - coef: " << this->output_coef[output_pin_list[i]] << "\n";
            }
        }
    }

    this->server_api->liveSetSeveralOutputServer(output_pin_list, output_value_list, output_name_list);
}

bool modeAudioLive::checkOutputCoef(std::vector<int> output_list)
{
    // delete old and build new coef list
    if (output_list.size() < 1)
        return false;

    bool same = true;
    if (output_list.size() == this->output_coef.size())
    {
        for(std::vector<int>::iterator it = output_list.begin(); it != output_list.end(); it++)
        {
            if (this->output_coef.find(*it) == this->output_coef.end())
            {
                same = false;
                break;
            }
        }
    }
    else
        same = false;

    if (same == false) // not same => reset
    {
        qInfo() << "checkOutputCoef will reset\n";
        this->outputSelectedChanged();
        this->output_coef.clear();
        for(std::vector<int>::iterator it = output_list.begin(); it != output_list.end(); it++)
            this->output_coef[*it] = 0;
        this->main_output_linear_pin = this->output_coef.begin()->first;
        this->output_coef.begin()->second = 1;
        return false;
    }
    return true;
}

void modeAudioLive::updateLinearChangeCoef()
{
    if (this->output_coef.size() < 1)
        return;

    for(std::map<int, float>::iterator it = this->output_coef.begin(); it != this->output_coef.end(); it++)
    {
        if (it->first == this->main_output_linear_pin)
        {
            bool need_change_main_pin = false;
            it->second -= LINEAR_CHANGE_RATE;
            if (it->second < 0)
            {
                it->second = 0;
                need_change_main_pin = true;
            }

            it++;
            if (it == this->output_coef.end())
                it = this->output_coef.begin();

            if (need_change_main_pin == true)
                this->main_output_linear_pin = it->first;

            it->second += LINEAR_CHANGE_RATE;
            if (it->second > 1)
                it->second = 1;
            break;
        }
    }
}

void modeAudioLive::updateMaxValueChange()
{
    if (this->output_coef.size() < 1)
        return;

    if (this->current_value >= this->max_value_change)
    {
        qint64 cur_time = QDateTime::currentMSecsSinceEpoch();
        if (cur_time >= this->last_time_max_change + MAX_VALUE_CHANGE_RATE_COOLDOWN_MS) {
            this->last_time_max_change = cur_time;
        }
        else
            return;

        for(std::map<int, float>::iterator it = this->output_coef.begin(); it != this->output_coef.end(); it++)
        {
            if (it->first == this->main_output_linear_pin)
            {
                float reminder = 0;
                it->second -= MAX_VALUE_CHANGE_RATE;
                if (it->second < 0)
                {
                    reminder = qAbs(it->second);
                    it->second = 0;
                }

                it++;
                if (it == this->output_coef.end())
                    it = this->output_coef.begin();

                if (reminder > 0)
                {
                    this->main_output_linear_pin = it->first;
                    it->second = 1;
                    it->second -= reminder;
                    it++;
                    if (it == this->output_coef.end())
                        it = this->output_coef.begin();
                    it->second += reminder;
                    if (it->second > 100)
                        it->second = 100;
                }
                else
                {
                    it->second += MAX_VALUE_CHANGE_RATE;
                    if (it->second > 100)
                        it->second = 100;
                }

                break;
            }
        }
    }
}

void modeAudioLive::updateAudioGui() {
    QLabel* max_label = live_audio_tab->findChild<QLabel*>("audio_max_label", Qt::FindChildrenRecursively);
    if (max_label)
        max_label->setText("Max value: " + QString::fromStdString(std::to_string(this->max_value)));

    QLabel* min_label = live_audio_tab->findChild<QLabel*>("audio_min_label", Qt::FindChildrenRecursively);
    if (min_label)
        min_label->setText("Min value: " + QString::fromStdString(std::to_string(this->min_value)));

    QLabel* raw_label = live_audio_tab->findChild<QLabel*>("audio_raw_value_label", Qt::FindChildrenRecursively);
    if (raw_label)
        raw_label->setText("Raw value: " + QString::fromStdString(std::to_string(this->raw_value)));

    QProgressBar* progress = live_audio_tab->findChild<QProgressBar*>("audio_current_value_progress", Qt::FindChildrenRecursively);
    if (progress)
        progress->setValue(this->current_value);

    QLCDNumber* lcd_num = live_audio_tab->findChild<QLCDNumber*>("audio_current_value_lcd", Qt::FindChildrenRecursively);
    if (lcd_num)
        lcd_num->display(this->current_value);
}

void modeAudioLive::resetTelemetry() {
    this->max_value = 0;
    this->min_value = -1;
    this->current_value = 0;
    this->updateAudioGui();
}

void modeAudioLive::changeStatus(AudioStatus status, QString added_text)
{
    this->status = status;
    QLabel* status_label = live_audio_tab->findChild<QLabel*>("live_audio_status_label", Qt::FindChildrenRecursively);
    if (status_label) {
        QString status_str = "NONE";
        if (status == AudioStatus::STARTING)
            status_str = "STARTING...";
        else if (status == AudioStatus::RUNNING)
            status_str = "RUNNING";
        else if (status == AudioStatus::STOPPING)
            status_str = "STOPPING...";
        else if (status == AudioStatus::STOPPED)
            status_str = "STOPPED";

        if (added_text.isEmpty() == false && added_text != "")
            status_str += " - " + added_text;

        status_label->setText("Status: " + status_str);
    }
}

void modeAudioLive::outputSelectedChanged()
{
    // put the unchecked pin to 0
    for(std::map<int, float>::iterator it = this->output_coef.begin(); it != this->output_coef.end(); it++)
    {
        std::map<int, QCheckBox*>::iterator it_check_list = this->live_output_list.begin();
        while (it_check_list != this->live_output_list.end())
        {
            if (it->first == it_check_list->first)
            {
                if (it_check_list->second->isChecked() == false)
                    this->mode_live->update_output_value(it->first, 0, false);

                break;
            }
            it_check_list++;
        }
    }
}

void modeAudioLive::setMaxAplitude() {
    switch (this->formatAudio.sampleSize()) {
    case 8:
        switch (this->formatAudio.sampleType()) {
        case QAudioFormat::UnSignedInt:
            this->format_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            this->format_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (this->formatAudio.sampleType()) {
        case QAudioFormat::UnSignedInt:
            this->format_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            this->format_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (this->formatAudio.sampleType()) {
        case QAudioFormat::UnSignedInt:
            this->format_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            this->format_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            this->format_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        break;
    }
}
