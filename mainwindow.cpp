#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionStartService, SIGNAL(triggered(bool)), this, SLOT(on_pushButtonStartService_clicked()));
    connect(ui->actionStopService, SIGNAL(triggered(bool)), this, SLOT(on_pushButtonStopService_clicked()));
    connect(ui->actionRestartService, SIGNAL(triggered(bool)), this, SLOT(on_pushButtonRestartService_clicked()));
    processMongodb = new QProcess(this);
    processNode = new QProcess(this);

    // dialog about
    DialogAbout * dlg = new DialogAbout();
    dlg->exec();
    delete dlg;



    // shortcut keys

    gbscStartService = new QxtGlobalShortcut(this);
    connect(gbscStartService, &QxtGlobalShortcut::activated, [=]() { on_pushButtonStartService_clicked();});
    gbscStopService = new QxtGlobalShortcut(this);
    connect(gbscStopService, &QxtGlobalShortcut::activated, [=]() { on_pushButtonStopService_clicked();});
    gbscRestartService = new QxtGlobalShortcut(this);
    connect(gbscRestartService, &QxtGlobalShortcut::activated, [=]() { on_pushButtonRestartService_clicked();});

    // configuration

    config = new Configuration(this);
    config->setConfigFilePath("config.dat");
    if (!config->loadConfiguration()) {
        on_actionConfiguration_triggered();
    }

    activateConfiguration();
    connect(ui->qxtGroupBoxMongodb, &QxtGroupBox::toggled, [=](bool check) {
        config->setChecked("Mongodb", check);
        config->saveConfiguration();
    });
    connect(ui->qxtGroupBoxNode, &QxtGroupBox::toggled, [=](bool check) {
        config->setChecked("Node", check);
        config->saveConfiguration();
    });


    // tray icon

    trayIcon = new TrayIcon(this);
    trayIconTimeOut = 2000;
    connect(trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        activateWindow();
        switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            showNormal();
            break;
        case QSystemTrayIcon::MiddleClick:
            trayIcon->showMessage("Service State", CurrentState(), QSystemTrayIcon::Information, trayIconTimeOut);
            break;
        default:
            break;
        }
    });
    QAction *actionQuit = new QAction("Quit(&Q)", this);
    connect(actionQuit, &QAction::triggered, [=](bool) {
        if (QMessageBox::question(this, "Web Control", "Are you sure you want to quit?") == QMessageBox::Yes) {
            config->saveConfiguration();
            qApp->quit();
        }
    });

    trayIcon->menuTrayIcon->addAction(ui->actionStartService);
    trayIcon->menuTrayIcon->addAction(ui->actionStopService);
    trayIcon->menuTrayIcon->addAction(ui->actionRestartService);
    trayIcon->menuTrayIcon->addSeparator();
    trayIcon->menuTrayIcon->addAction(ui->actionStartNpmInstall);
    trayIcon->menuTrayIcon->addSeparator();
    trayIcon->menuTrayIcon->addAction(actionQuit);

    trayIcon->showMessage("Web Control", "Welcome!\nI am here, too!\nHave a nice time!", QSystemTrayIcon::Information, 1000);
    updateTrayIconToolTip();

    // main

    //    connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(slot_watcher_fileChanged(QString)));
    //    connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(slot_watcher_directoryChanged(QString)));

    connect(processMongodb, SIGNAL(readyRead()), this, SLOT(slot_processMongodb_readyRead()));
    connect(processMongodb, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slot_processMongodb_finished(int,QProcess::ExitStatus)));
    connect(processMongodb, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(slot_processMongodb_stateChanged(QProcess::ProcessState)));

//    connect(&processNode, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_processNode_readyReadStandardOutput()));
//    connect(&processNode, SIGNAL(readyReadStandardError()), this, SLOT(slot_processNode_readyReadStandardError()));
    connect(processNode, SIGNAL(readyRead()), this, SLOT(slot_processNode_readyRead()));
    connect(processNode, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slot_processNode_finished(int,QProcess::ExitStatus)));
    connect(processNode, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(slot_processNode_stateChanged(QProcess::ProcessState)));
    connect(processNode, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(slot_processNode_errorOccured(QProcess::ProcessError)));

    boolServiceHasToStop = false;
}


MainWindow::~MainWindow()
{
    if (processMongodb->state() != QProcess::NotRunning)
        GentleKillProcess(processMongodb);
    if (processNode->state() != QProcess::NotRunning)
        GentleKillProcess(processNode);
    delete ui;
}
// H:\blog\__running\node.exe H:/blog/__running/../bin/www
// H:\blog\__running\supervisor.cmd -w H:/blog/__running/../ H:/blog/__running/../bin/www
// H:\blog\__running\node.exe H:/blog/__running/node_modules/supervisor/lib/cli-wrapper.js -w H:/blog/__running/../ H:/blog/__running/../bin/www
// H:\blog\__running\supervisor.cmd -w H:/blog/__running/../ H:/blog/__running/../bin/www
// node.exe  node_modules\supervisor\lib\cli-wrapper.js -w ..\ ..\bin\www
//H:/blog/__running/mongod.exe --dbpath=h:\blog\mongodb

void MainWindow::StartProcessMongodb_NoSecure() {
    ui->textBrowserMongodbStatus->clear();
    QString pathMongodb = config->path("Mongodb");
    processMongodb->start("__essential/mongod.exe", QStringList(tr("--dbpath=%1").arg(pathMongodb)));
}
void MainWindow::StartProcessNode_NoSecure() {
    ui->textBrowserNodeStatus->clear();
    QString pathProjectFolder = config->path("Project Folder");
    processNode->start("__essential/node.exe", QStringList() << tr("%1/bin/www").arg(pathProjectFolder));
}

void MainWindow::on_pushButtonStartService_clicked()
{
    qDebug() << "Start Service...";
    boolServiceHasToStop = false;

    QString pathProjectFolder, pathMongodb;
    pathProjectFolder = config->path("Project Folder");
    pathMongodb = config->path("Mongodb");

    if (ui->qxtGroupBoxMongodb->isChecked()) {
        if (!QFile::exists(pathMongodb)) {
            QMessageBox::warning(this, "Error", "Selected \"Mongodb\" doesn't exists!\nPlease Reset Configuration...");
            on_actionConfiguration_triggered();
            return;
        }
        if (!QFile::exists("__essential/mongod.exe")) {
            QMessageBox::warning(this, "File Not Exist", "File \"/__essential/mongod.exe\" doesn't exist in your directory! Please Copy it to current directory!");
            return;
        }
        if (processMongodb->state() == QProcess::NotRunning) {
            StartProcessMongodb_NoSecure();
        }
    }
    if (ui->qxtGroupBoxNode->isChecked()) {
        if (!QFile::exists(pathProjectFolder)) {
            QMessageBox::warning(this, "Error", "Selected \"Project Folder\" doesn't exists!\nPlease Reset Configuration...");
            on_actionConfiguration_triggered();
            return;
        }

        if (!QFile::exists("__essential/node.exe")) {
            QMessageBox::warning(this, "File Not Exist", "File \"/__essential/node.exe\" doesn't exist in your directory! Please Copy it to current directory!");
            return;
        }

        if (processNode->state() == QProcess::NotRunning) {
            StartProcessNode_NoSecure();
                //        processNode.start("__essential/supervisor.cmd", QStringList() << "-w" << ui->lineEditProjectFolder->text() << tr("%1/bin/www").arg(ui->lineEditProjectFolder->text()),
            //                          QProcess::WriteOnly);

            //        processNode.start("__essential/node.exe", QStringList() <<  tr("_essential/node_modules/supervisor/lib/cli-wrapper.js") << "-w" << ui->lineEditProjectFolder->text() << tr("%1/bin/www").arg(ui->lineEditProjectFolder->text()),
            //                          QProcess::WriteOnly);

            //        processNode.setWorkingDirectory(pathProjectFolder);
            //        processNode.start(QDir::currentPath() + "/__essential/npm.cmd", QStringList() << "start");
            //        processNode.start(dir.path() + "/d.bat", QProcess::WriteOnly);
        }
    }


    //    processNode.start("cmd.exe",
    //                      QStringList() << "/c" << dir.path() + "/node.exe" << tr("%1\\node_modules\\supervisor\\lib\\cli-wrapper.js").arg(dir.path()) << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path()));
/*
    processNode.setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        //        args->flags |= CREATE_NEW_CONSOLE;
        //        args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        //        args->startupInfo->dwFlags |= STARTF_USEFILLATTRIBUTE;
        //        args->startupInfo->dwFillAttribute = BACKGROUND_BLUE | FOREGROUND_RED
        //                                           | FOREGROUND_INTENSITY;
    });
*/
    //    processNode.closeWriteChannel();
    //    processNode.start(dir.path() + "/supervisor.cmd",
    //                      QStringList() << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path()));

    //    processNode.startDetached(dir.path() + "/node.exe", QStringList() <<  tr("%1\\node_modules\\supervisor\\lib\\cli-wrapper.js").arg(dir.path()) << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path()), NULL, &pidNode);
//    processNode.start(dir.path() + "/node.exe", QStringList() <<  tr("%1\\node_modules\\supervisor\\lib\\cli-wrapper.js").arg(dir.path()) << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path()));
    //    processNode.start("supervisor", QStringList()  << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path()));

    //    processNode.setCurrentWriteChannel(NULL)
//    processNode.start(dir.path() + "/node.exe", QStringList() << tr("%1\\..\\bin\\www").arg(dir.path()));


    //    qDebug() <<dir.path() + "/node.exe" << tr("%1\\node_modules\\supervisor\\lib\\cli-wrapper.js").arg(dir.path()) << "-w" << tr("%1\\..\\").arg(dir.path()) << tr("%1\\..\\bin\\www").arg(dir.path());


}



void MainWindow::slot_processMongodb_readyRead() {
    ui->textBrowserMongodbStatus->append(QString::fromLocal8Bit(processMongodb->readAll()));
}
void MainWindow::slot_processMongodb_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Mongodb finished: " << ";  exitCode: " << exitCode << ";  ExitStatus: " << exitStatus;
}
void MainWindow::slot_processMongodb_stateChanged(QProcess::ProcessState newState) {
    QString state;
    switch (newState) {
    case QProcess::NotRunning:
        state = "Not Running";
        break;
    case QProcess::Starting:
        state = "Starting";
        break;
    case QProcess::Running:
        state = "Running";
        break;
    }
    qDebug() << "Mongodb State Changed: " << state;
    ui->labelStatusMongodb->setText(state);
    updateTrayIconToolTip();
    if (boolServiceHasToStop == false && newState == QProcess::NotRunning)
        StartProcessMongodb_NoSecure();
//    trayIcon->showMessage("Service State Changed", CurrentState(), QSystemTrayIcon::Information, trayIconTimeOut);
}


/*
void MainWindow::slot_processNode_readyReadStandardOutput() {
    QString str = QString::fromLocal8Bit(processNode.readAllStandardOutput());
//    QString str = processNode.readAllStandardOutput();
    ui->textBrowserNodeStatus->append(str);
    qDebug() << "Node Standard Output: " << str;
}
void MainWindow::slot_processNode_readyReadStandardError() {
    QString str = QString::fromLocal8Bit(processNode.readAllStandardError());
    ui->textBrowserNodeStatus->append(str);
    qDebug() << "Node Standard Error: " << str;
}*/

void MainWindow::slot_processNode_readyRead() {
    QString str = QString::fromLocal8Bit(processNode->readAll());
    ui->textBrowserNodeStatus->append(str);
    qDebug() << "Node Output: " << str;
}
void MainWindow::slot_processNode_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Node finished! " << ";  exitCode: " << exitCode << ";  ExitStatus: " << exitStatus;
}
void MainWindow::slot_processNode_stateChanged(QProcess::ProcessState newState) {
    QString state;
    switch (newState) {
    case QProcess::NotRunning:
        state = "Not Running";
        break;
    case QProcess::Starting:
        state = "Starting";
        break;
    case QProcess::Running:
        state = "Running";
        break;
    }
    qDebug() << "Node State Changed: " << state;
    ui->labelStatusNode->setText(state);
    updateTrayIconToolTip();
    if (boolServiceHasToStop == false && newState == QProcess::NotRunning)
        StartProcessNode_NoSecure();
//    trayIcon->showMessage("Service State Changed", CurrentState(), QSystemTrayIcon::Information, trayIconTimeOut);
}

void MainWindow::slot_processNode_errorOccured(QProcess::ProcessError err) {
    qDebug() << "Node Error Occured! QProcess::ProcessError: " << err << "; errorString: " << processNode->errorString();
}

/*
void MainWindow::slot_watcher_fileChanged(QString str) {
    qDebug() << str;
}
void MainWindow::slot_watcher_directoryChanged(QString str) {
    qDebug() << str;
    qDebug() << "Restart!!!";
    QDir dir(pathHome);
    GentleKillProcess(processNode);
    processNode.start(dir.path() + "/node.exe",
                      QStringList() << tr("%1\\..\\bin\\www").arg(dir.path()));
}
*/


void MainWindow::on_pushButtonStopService_clicked()
{
    qDebug() << "Stop Service...";
    boolServiceHasToStop = true;
    if (processMongodb->state() == QProcess::Running) {
        GentleKillProcess(processMongodb);
    }
    if (processNode->state() == QProcess::Running) {
        GentleKillProcess(processNode);
    }
//    processNode.terminate();

}

void MainWindow::on_pushButtonRestartService_clicked()
{
    on_pushButtonStopService_clicked();
    on_pushButtonStartService_clicked();
}



void MainWindow::GentleKillProcess(QProcess * process)
{

    AttachConsole(process->processId()); // attach to process console
    SetConsoleCtrlHandler(NULL, TRUE); // disable Control+C handling for our app
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); // generate Control+C event


    if (process == processNode) {
        process->waitForFinished(100);
        if (process->state() != QProcess::NotRunning) {
            process->write(QByteArray("Y\n"));
            process->waitForFinished(2000);
        }
    }
    else
        process->waitForFinished(2000);

//     process.waitForFinished(2000);
    SetConsoleCtrlHandler(NULL, FALSE); // disable Control+C handling for our app
    FreeConsole();

}
QString MainWindow::CurrentState() {
    QString state = "MongoDB:";
    switch (processMongodb->state()) {
    case QProcess::NotRunning:
        state += "Not Running";
        break;
    case QProcess::Starting:
        state += "Starting";
        break;
    case QProcess::Running:
        state += "Running";
    }
    state += "\n";

    state += "Node JS:";
    switch (processNode->state()) {
    case QProcess::NotRunning:
        state += "Not Running";
        break;
    case QProcess::Starting:
        state += "Starting";
        break;
    case QProcess::Running:
        state += "Running";
    }

    return state;
}

// tray icon
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        trayIcon->showMessage("Service State", CurrentState(), QSystemTrayIcon::Information, trayIconTimeOut);
        hide();
        event->ignore();
    }
    else
        event->accept();
}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout * dlg = new DialogAbout();
    dlg->exec();
    delete dlg;

}

void MainWindow::on_actionConfiguration_triggered()
{
    DialogConfiguration *dlg = new DialogConfiguration(config);
    if (dlg->exec() == QDialog::Accepted) {
        activateConfiguration();
    }
    delete dlg;
}
void MainWindow::activateConfiguration() {
    gbscStartService->setShortcut(config->keySequence("Start Service"));
    gbscStopService->setShortcut(config->keySequence("Stop Service"));
    gbscRestartService->setShortcut(config->keySequence("Restart Service"));
    ui->qxtGroupBoxMongodb->setChecked(config->checked("Mongodb"));
    ui->qxtGroupBoxNode->setChecked(config->checked("Node"));
}
void MainWindow::updateTrayIconToolTip() {
    trayIcon->setToolTip("Web Control\nStatus\n" + CurrentState());
}

void MainWindow::on_actionStartNpmInstall_triggered()
{
    QProcess *process = new QProcess(this);
    QString pathProjectFolder = config->path("Project Folder");
    process->startDetached("cmd", QStringList() << "/k" << "cd" << "/d" << pathProjectFolder << "&&" << QDir::currentPath() + "/__essential/npm.cmd" << "install");
}
