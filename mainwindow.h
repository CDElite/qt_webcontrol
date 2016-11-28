#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QTime>
#include <QDebug>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QCloseEvent>
#include "trayicon.h"
#include "dialogabout.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include "configuration.h"
#include "dialogconfiguration.h"
#include "qxtglobalshortcut.h"
#include <qxtconfirmationmessage.h>
#include <QTranslator>
#include <QLocale>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // tray icon
private:
    TrayIcon *trayIcon;
    int trayIconTimeOut;
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
private:
    void updateTrayIconToolTip();

    // configuration
private:
    Configuration *config;

    // shortcut keys...
private:
    QxtGlobalShortcut * gbscStartService;
    QxtGlobalShortcut * gbscStopService;
    QxtGlobalShortcut * gbscRestartService;
private:
    void activateConfiguration();

    // translate

    // main part

private slots:
    void on_pushButtonStartService_clicked();
    void on_pushButtonStopService_clicked();

    void slot_processMongodb_readyRead();
    void slot_processMongodb_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void slot_processMongodb_stateChanged(QProcess::ProcessState newState);

    void slot_processNode_stateChanged(QProcess::ProcessState newState);
    void slot_processNode_readyRead();
//    void slot_processNode_readyReadStandardOutput();
//    void slot_processNode_readyReadStandardError();
    void slot_processNode_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void slot_processNode_errorOccured(QProcess::ProcessError);

//    void slot_watcher_fileChanged(QString);
//    void slot_watcher_directoryChanged(QString);


    void on_actionAbout_triggered();

    void on_pushButtonRestartService_clicked();

    void on_actionConfiguration_triggered();


    void on_actionStartNpmInstall_triggered();

private:
    Ui::MainWindow *ui;
    QProcess *processMongodb, *processNode;
    bool boolServiceHasToStop;
//    QFileSystemWatcher watcher;
//    qint64 pidNode;


private:
    void StartProcessMongodb_NoSecure();
    void StartProcessNode_NoSecure();
    void GentleKillProcess(QProcess *);
    QString CurrentState();
};

#endif // MAINWINDOW_H
