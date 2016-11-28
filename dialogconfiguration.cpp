#include "dialogconfiguration.h"
#include "ui_dialogconfiguration.h"

DialogConfiguration::DialogConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfiguration)
{
    ui->setupUi(this);
}
DialogConfiguration::DialogConfiguration(Configuration *cfg) :
    QDialog(),
    ui(new Ui::DialogConfiguration)
{
    ui->setupUi(this);
    connect(ui->pushButtonProjectFolder, &QPushButton::clicked, [=]() {
        QString strFolder = QFileDialog::getExistingDirectory(this, "Select the project folder", ui->lineEditProjectFolder->text());
        if (!strFolder.isNull()) {
            ui->lineEditProjectFolder->setText(strFolder);
        }
    });
    connect(ui->pushButtonMongodb, &QPushButton::clicked, [=]() {
        QString strFolder = QFileDialog::getExistingDirectory(this, "Select the Mongodb folder", ui->lineEditMongodb->text());
        if (!strFolder.isNull()) {
            ui->lineEditMongodb->setText(strFolder);
        }
    });
    config = cfg;
    loadConfigFile();

}

DialogConfiguration::~DialogConfiguration()
{
    delete ui;
}

void DialogConfiguration::loadConfigFile() {
    if (!config->loadConfiguration()) {
        QMessageBox::warning(NULL, "Loading Error", "Loading Configuration File Failed!\nError : " + config->errorString() + "\nCreate new configuration file...");
        ui->keySequenceEditStart->setKeySequence(QKeySequence("Ctrl+F1"));
        ui->keySequenceEditStop->setKeySequence(QKeySequence("Ctrl+F2"));
        ui->keySequenceEditRestart->setKeySequence(QKeySequence("Ctrl+F3"));
        return;
    }
    // Your code here...
    ui->lineEditProjectFolder->setText(config->path("Project Folder"));
    ui->lineEditMongodb->setText(config->path("Mongodb"));
    ui->keySequenceEditStart->setKeySequence(config->keySequence("Start Service"));
    ui->keySequenceEditStop->setKeySequence(config->keySequence("Stop Service"));
    ui->keySequenceEditRestart->setKeySequence(config->keySequence("Restart Service"));
    ui->checkBoxMongodb->setChecked(config->checked("Mongodb"));
    ui->checkBoxNode->setChecked(config->checked("Node"));
    // ...
}
void DialogConfiguration::saveConfigFile() {
    // Your code here...
    config->setPath("Project Folder", ui->lineEditProjectFolder->text());
    config->setPath("Mongodb", ui->lineEditMongodb->text());
    config->setKeySequence("Start Service", ui->keySequenceEditStart->keySequence());
    config->setKeySequence("Stop Service", ui->keySequenceEditStop->keySequence());
    config->setKeySequence("Restart Service", ui->keySequenceEditRestart->keySequence());
    config->setChecked("Mongodb", ui->checkBoxMongodb->isChecked());
    config->setChecked("Node", ui->checkBoxNode->isChecked());
    // ...
    if (!config->saveConfiguration()) {
        QMessageBox::warning(this, "Saving Error", "Saving Configuration File Failed!\nError : " + config->errorString());
        return;
    }
}

void DialogConfiguration::on_pushButtonOK_clicked()
{
    // Your code here...
    QString errorPath;
    if (!QFile::exists(ui->lineEditProjectFolder->text()))
        errorPath.append("Project Folder\n");
    if (!QFile::exists(ui->lineEditMongodb->text()))
        errorPath.append("Mongodb\n");
    if (!errorPath.isEmpty()) {
        QMessageBox::warning(this, "File Path Error", "Path doesn't exist:\n" + errorPath);
        return;
    }
    // ...

    saveConfigFile();
    accept();
}

void DialogConfiguration::on_pushButtonLoadDefault_clicked()
{
    loadConfigFile();
}
