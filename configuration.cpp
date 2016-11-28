#include "configuration.h"

Configuration::Configuration(QObject *parent) : QObject(parent)
{
    currentVersion = 3;
}

void Configuration::setConfigFilePath(const QString& path) {
    stringConfigFilePath = path;
}
QString Configuration::errorString() {
    return stringError;
}


bool Configuration::saveConfiguration() {
    QFile file(stringConfigFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        stringError = "File Open Access Failed";
        emit errorOccured(FILE_OPEN_ACCESS_FAILED);
        return false;
    }
    QDataStream dataStream(&file);


    dataStream << (quint32)0x19980128;      //magic number
    dataStream << (qint32)currentVersion;   // version

    // Your data here...
    dataStream << mapPaths;
    dataStream << mapKeySequence;
    dataStream << mapChecked;
    // ...

    file.close();
    return true;
}
bool Configuration::loadConfiguration() {
    if (!QFile::exists(stringConfigFilePath)) {
        stringError = "File Not Exist";
        emit errorOccured(FILE_NOT_EXIST);
        return false;
    }

    QFile file(stringConfigFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        stringError = "File Open Access Failed";
        emit errorOccured(FILE_OPEN_ACCESS_FAILED);
        return false;
    }
    QDataStream dataStream(&file);

    quint32 magicNumber;
    dataStream >> magicNumber;
    if (magicNumber != 0x19980128) {
        stringError = "File Bad Format";
        emit errorOccured(FILE_BAD_FORMAT);
        return false;
    }
    qint32 version;
    dataStream >> version;
    if (version < currentVersion) {
        stringError = "File Old Version";
        emit errorOccured(FILE_VERSION_OLD);
        return false;
    }
    if (version > currentVersion) {
        stringError = "File New Version";
        emit errorOccured(FILE_VERSION_NEW);
        return false;
    }

    // Your data here...

    dataStream >> mapPaths;
    dataStream >> mapKeySequence;
    dataStream >> mapChecked;

    // ...

    file.close();
    return true;
}
// Your configurations here...
void Configuration::setPath(const QString& title, const QString& path) {
    mapPaths[title] = path;
}
QString Configuration::path(const QString &title) {
    return mapPaths[title];
}
void Configuration::setKeySequence(const QString& title, const QKeySequence& key) {
    mapKeySequence[title] = key;
}
QKeySequence Configuration::keySequence(const QString& title) {
    return mapKeySequence[title];
}
void Configuration::setChecked(const QString& title, bool key) {
    mapChecked[title] = key;
}

bool Configuration::checked(const QString& title) {
    return mapChecked[title];
}
