#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <iostream>
#include <string>
#include <map>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <exception>

#define FILE_NAME "settings.json"

class SettingsManager
{
public:
    enum SettingField {
        LAST_IP_USED,
        LAST_PORT_USED,
        CONECTION_ARRAY,
        CONECTION_ARR_IP,
        CONECTION_ARR_PORT,
        CONECTION_ARR_NAME,
        AUDIO_LAST_AUDIOINPUT,
        AUDIO_LAST_GAIN,
        AUDIO_LAST_MULTIPLIER,
        AUDIO_LAST_USE_NORMALIZE,
        AUDIO_LAST_USE_LINEAR,
        AUDIO_LAST_USE_MAX_VALUE_CHANGE,
        AUDIO_LAST_MAX_VALUE_CHANGE,
        SETTING_LIVE_INPUT_POLL_TIME,
        SETTING_AUDIO_UPDATE_INTERVAL,
        SETTING_AUDIO_NORMALIZE_TIMEOUT,
        SETTING_AUDIO_NORMALIZE_MIN_VALUE,
        SETTING_AUDIO_LINEAR_CHANGE_RATE,
        SETTING_AUDIO_MAX_VALUE_CHANGE_RATE,
        SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN,
    };

public:
    SettingsManager();

    QString getDataStr(SettingField field);
    bool getDataBool(SettingField field);
    int getDataInt(SettingField field);
    double getDataDouble(SettingField field);
    QJsonArray getArray(SettingField field);
    QString getFieldStr(SettingField field);

    bool setData(SettingField field, QString data);
    bool setData(SettingField field, bool data);
    bool setData(SettingField field, int data);
    bool setData(SettingField field, double data);
    bool setData(SettingField field, QJsonArray data);
    //bool addToArray();
    //bool removeFromArray();

    QString getDefaultStrValue(SettingField field);
    bool getDefaultBoolValue(SettingField field);
    int getDefaultIntValue(SettingField field);
    double getDefaultDoubleValue(SettingField field);

private:
    bool readFromFile();
    bool writeToFile();

private:
    QJsonDocument json_doc;

    const std::map<SettingField, QString> field_name = {
        {LAST_IP_USED, "LAST_IP_USED"},
        {LAST_PORT_USED, "LAST_PORT_USED"},
        {CONECTION_ARRAY, "CONECTION_ARRAY"},
        {CONECTION_ARR_IP, "CONECTION_ARR_IP"},
        {CONECTION_ARR_PORT, "CONECTION_ARR_PORT"},
        {CONECTION_ARR_NAME, "CONECTION_ARR_NAME"},
        {AUDIO_LAST_AUDIOINPUT, "AUDIO_LAST_AUDIOINPUT"},
        {AUDIO_LAST_GAIN, "AUDIO_LAST_GAIN"},
        {AUDIO_LAST_MULTIPLIER, "AUDIO_LAST_MULTIPLIER"},
        {AUDIO_LAST_USE_NORMALIZE, "AUDIO_LAST_USE_NORMALIZE"},
        {AUDIO_LAST_USE_LINEAR, "AUDIO_LAST_USE_LINEAR"},
        {AUDIO_LAST_USE_MAX_VALUE_CHANGE, "AUDIO_LAST_USE_MAX_VALUE_CHANGE"},
        {AUDIO_LAST_MAX_VALUE_CHANGE, "AUDIO_LAST_MAX_VALUE_CHANGE"},
        {SETTING_LIVE_INPUT_POLL_TIME, "SETTING_LIVE_INPUT_POLL_TIME"},
        {SETTING_AUDIO_UPDATE_INTERVAL, "SETTING_AUDIO_UPDATE_INTERVAL"},
        {SETTING_AUDIO_NORMALIZE_TIMEOUT, "SETTING_AUDIO_NORMALIZE_TIMEOUT"},
        {SETTING_AUDIO_NORMALIZE_MIN_VALUE, "SETTING_AUDIO_NORMALIZE_MIN_VALUE"},
        {SETTING_AUDIO_LINEAR_CHANGE_RATE, "SETTING_AUDIO_LINEAR_CHANGE_RATE"},
        {SETTING_AUDIO_MAX_VALUE_CHANGE_RATE, "SETTING_AUDIO_MAX_VALUE_CHANGE_RATE"},
        {SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN, "SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN"}
    };

    const std::map<SettingField, QString> default_str_value = {
        {LAST_IP_USED, "127.0.0.1"},
        {LAST_PORT_USED, "4242"},
        {AUDIO_LAST_AUDIOINPUT, ""}
    };

    const std::map<SettingField, bool> default_bool_value = {
        {AUDIO_LAST_USE_NORMALIZE, false},
        {AUDIO_LAST_USE_LINEAR, false},
        {AUDIO_LAST_USE_MAX_VALUE_CHANGE, false}
    };

    const std::map<SettingField, int> default_int_value = {
        {AUDIO_LAST_GAIN, 0},
        {AUDIO_LAST_MAX_VALUE_CHANGE, 80},
        {SETTING_LIVE_INPUT_POLL_TIME, 2000},
        {SETTING_AUDIO_UPDATE_INTERVAL, 50},
        {SETTING_AUDIO_NORMALIZE_TIMEOUT, 2000},
        {SETTING_AUDIO_NORMALIZE_MIN_VALUE, 20},
        {SETTING_AUDIO_MAX_VALUE_CHANGE_COOLDOWN, 250}
    };

    const std::map<SettingField, double> default_double_value = {
        {AUDIO_LAST_MULTIPLIER, 1.0},
        {SETTING_AUDIO_LINEAR_CHANGE_RATE, 0.02},
        {SETTING_AUDIO_MAX_VALUE_CHANGE_RATE, 0.50}
    };
};

#endif // SETTINGSMANAGER_H
