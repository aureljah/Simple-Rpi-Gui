#include "settingsmanager.hpp"

SettingsManager::SettingsManager()
{
    this->readFromFile();
}

bool SettingsManager::readFromFile()
{
    QFile file(FILE_NAME);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
        return false;
    QJsonParseError JsonParseError;
    this->json_doc = QJsonDocument::fromJson(file.readAll(), &JsonParseError);
    file.close();
    return true;
}

bool SettingsManager::writeToFile()
{
    QFile file(FILE_NAME);
    if (file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate) == false)
        return false;
    file.write(this->json_doc.toJson());
    file.close();
    return true;
}

bool SettingsManager::setData(SettingField field, QString data)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    RootObject.insert(field_str, data);

    this->json_doc.setObject(RootObject); // set to json document
    this->writeToFile();
    return true;
}

bool SettingsManager::setData(SettingField field, bool data)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    RootObject.insert(field_str, data);

    this->json_doc.setObject(RootObject); // set to json document
    this->writeToFile();
    return true;
}

bool SettingsManager::setData(SettingField field, int data)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    RootObject.insert(field_str, data);

    this->json_doc.setObject(RootObject); // set to json document
    this->writeToFile();
    return true;
}

bool SettingsManager::setData(SettingField field, double data)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    RootObject.insert(field_str, data);

    this->json_doc.setObject(RootObject); // set to json document
    this->writeToFile();
    return true;
}

bool SettingsManager::setData(SettingField field, QJsonArray data)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    RootObject.insert(field_str, data);

    this->json_doc.setObject(RootObject); // set to json document
    this->writeToFile();
    return true;
}

QString SettingsManager::getDataStr(SettingField field)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    if (RootObject.contains(field_str) == true)
    {
        QJsonValueRef ref = RootObject.find(field_str).value();
        qInfo() << "getDataStr: " << field_str << " => " << ref.toString() << "\n";
        return ref.toString();
    }
    else
        return this->getDefaultStrValue(field);
}

bool SettingsManager::getDataBool(SettingField field)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    if (RootObject.contains(field_str) == true)
    {
        QJsonValueRef ref = RootObject.find(field_str).value();
        qInfo() << "getDataBool: " << field_str << " => " << ref.toBool() << "\n";
        return ref.toBool();
    }
    else
        return this->getDefaultBoolValue(field);
}

int SettingsManager::getDataInt(SettingField field)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    if (RootObject.contains(field_str) == true)
    {
        QJsonValueRef ref = RootObject.find(field_str).value();
        qInfo() << "getDataInt: " << field_str << " => " << ref.toInt() << "\n";
        return ref.toInt();
    }
    else
        return this->getDefaultIntValue(field);
}

double SettingsManager::getDataDouble(SettingField field)
{
    QString field_str = this->getFieldStr(field);

    QJsonObject RootObject = this->json_doc.object();
    if (RootObject.contains(field_str) == true)
    {
        QJsonValueRef ref = RootObject.find(field_str).value();
        qInfo() << "getDataDouble: " << field_str << " => " << ref.toDouble() << "\n";
        return ref.toDouble();
    }
    else
        return this->getDefaultDoubleValue(field);
}

QJsonArray SettingsManager::getArray(SettingField field)
{
    QString field_str = this->getFieldStr(field);
    QJsonArray array;

    QJsonObject RootObject = this->json_doc.object();
    if (RootObject.contains(field_str) == true)
    {
        QJsonValueRef ref = RootObject.find(field_str).value();
        array = ref.toArray();
        qInfo() << "getArray: " << field_str << " => " << array << "\n";
        return array;
    }
    else
        return array;
}

/*
QJsonObject RootObject = JsonDocument.object();
QJsonValueRef ref = RootObject.find("Address").value();
QJsonObject m_addvalue = ref.toObject();
m_addvalue.insert("Street","India");//set the value you want to modify
ref=m_addvalue; //assign the modified object to reference
JsonDocument.setObject(RootObject); // set to json document
*/

QString SettingsManager::getFieldStr(SettingField field)
{
    return this->field_name.at(field);
}
QString SettingsManager::getDefaultStrValue(SettingField field)
{
    return this->default_str_value.at(field);
}
bool SettingsManager::getDefaultBoolValue(SettingField field)
{
    return this->default_bool_value.at(field);
}
int SettingsManager::getDefaultIntValue(SettingField field)
{
    return this->default_int_value.at(field);
}
double SettingsManager::getDefaultDoubleValue(SettingField field)
{
    return this->default_double_value.at(field);
}
