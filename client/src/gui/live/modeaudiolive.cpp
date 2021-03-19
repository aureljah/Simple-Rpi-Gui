#include "modeaudiolive.h"

modeAudioLive::modeAudioLive(QWidget *live_audio_tab, modeLive *mode_live)
    : live_audio_tab(live_audio_tab), mode_live(mode_live), audioInput(nullptr), buffer_process_thread(nullptr),
    current_value(0), max_value(0), min_value(-1)
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

    this->setUpAudio(inputDevice);
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
        this->audioInput->stop();
        delete this->audioInput;
        this->audioInput = nullptr;
    }
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
            /*QObject::connect(check, &QCheckBox::toggled,
                             this, &modeAudioLive::outputSelectedChanged);*/
        }
        qInfo() << "updateOutputSelect: finsihed add and update title: live_output_list.size: " << this->live_output_list.size() << "\n";
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

void modeAudioLive::setUpAudio(QAudioDeviceInfo inputDevice)
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
    }
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
        qint64 bytesReady = this->audioInput->bytesReady();

        for (int i = 0 ; i < BUFFER_SIZE + 2; i++)
        {   buffer[i] = 0;  }

        qint64 readed = this->audioDevice->read(buffer, BUFFER_SIZE);
        if (readed > 0)
        {
            qInfo() << "buffer_processing: readed: " << readed << " - bytesReady: " << bytesReady << "\n";

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

void modeAudioLive::updateAudioValue(int value)
{
    // convert to %
    //value = value - 128;
    //value = qAbs(value);
    //value = floor(((float)value / (float)128) * 100);
    qInfo() << "updateAudioValue: value: " << value << "\n";

    this->current_value = value;
    if (value < this->min_value || this->min_value == -1)
        this->min_value = value;
    if (value > this->max_value)
        this->max_value = value;

    this->updateAudioGui();

    this->useCheckedOutput();
}

void modeAudioLive::useCheckedOutput()
{
    std::list<int> output_pin_list;
    std::map<int, QCheckBox*>::iterator it = this->live_output_list.begin();
    while (it != this->live_output_list.end()) {
        if (it->second->isChecked() == true) {
            output_pin_list.push_back(it->first);
        }
        it++;
    }

    for(std::list<int>::iterator it = output_pin_list.begin(); it != output_pin_list.end(); it++) {
        this->mode_live->update_output_value(*it, this->current_value);
    }
}

void modeAudioLive::updateAudioGui() {
    QLabel* max_label = live_audio_tab->findChild<QLabel*>("audio_max_label", Qt::FindChildrenRecursively);
    if (max_label)
        max_label->setText("Max value: " + QString::fromStdString(std::to_string(this->max_value)));

    QLabel* min_label = live_audio_tab->findChild<QLabel*>("audio_min_label", Qt::FindChildrenRecursively);
    if (min_label)
        min_label->setText("Min value: " + QString::fromStdString(std::to_string(this->min_value)));

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
