#ifndef DIALOGCONFIGURATION_H
#define DIALOGCONFIGURATION_H

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include "configuration.h"
#include <QList>

namespace Ui {
class DialogConfiguration;
}

class DialogConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfiguration(QWidget *parent);
    explicit DialogConfiguration(Configuration *cfg);
    ~DialogConfiguration();

private slots:
    void on_pushButtonOK_clicked();

    void on_pushButtonLoadDefault_clicked();

private:
    Ui::DialogConfiguration *ui;
    Configuration *config;

private:
    void loadConfigFile();
    void saveConfigFile();
};

#endif // DIALOGCONFIGURATION_H
