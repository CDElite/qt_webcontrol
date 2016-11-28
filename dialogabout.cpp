#include "dialogabout.h"
#include "ui_dialogabout.h"

#include <QDebug>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    connect(ui->pushButtonChat_1, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl("tencent://Message/?Uin=3192053831"));
    });
    connect(ui->pushButtonAdd_1, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl("tencent://addcontact/?fromId=50&fromSubId=1&subcmd=all&uin=3192053831"));
    });
    connect(ui->pushButtonChat_2, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl("tencent://Message/?Uin=3210608578"));
    });
    connect(ui->pushButtonAdd_2, &QPushButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl("tencent://addcontact/?fromId=50&fromSubId=1&subcmd=all&uin=3210608578"));
    });

}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    QDesktopServices::openUrl(arg1);
    qDebug() << arg1;
}
