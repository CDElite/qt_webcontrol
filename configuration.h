#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QMap>
#include <QKeySequence>

class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = 0);
    void setConfigFilePath(const QString& path);
    bool saveConfiguration();
    bool loadConfiguration();
    QString errorString();
public:
    enum ErrorSignal {
        FILE_NOT_EXIST, FILE_BAD_FORMAT, FILE_VERSION_OLD, FILE_VERSION_NEW, FILE_OPEN_ACCESS_FAILED
    };
signals:
    void saveSuccessful();
    void errorOccured(ErrorSignal);

private:
    QString stringConfigFilePath;
    QString stringError;
    int currentVersion;

    // Your configuration data here...
private:
    QMap<QString, QString> mapPaths;
    QMap<QString, QKeySequence> mapKeySequence;
    QMap<QString, bool> mapChecked;
public:
    void setPath(const QString& title, const QString& path);
    QString path(const QString& title);
    void setKeySequence(const QString& title, const QKeySequence& key);
    QKeySequence keySequence(const QString& title);
    void setChecked(const QString& title, bool key);
    bool checked(const QString& title);
    // ...

};

#endif // CONFIGURATION_H

/*
 * Version 1
 * mapPaths(Project Name, Mongodb)
 *
 * Version 2
 * mapPaths(Project Name, Mongodb)
 * mapKeySequence
 * */
