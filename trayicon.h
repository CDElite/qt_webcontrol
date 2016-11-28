#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>
#include <QAction>
#include <QtGui>
#include <QMenu>
#include <QWidget>
#include <QApplication>
#include <QObject>

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    TrayIcon();
    TrayIcon(QObject *parent);

private:
    QSystemTrayIcon *trayIcon;
public:
    QMenu *menuTrayIcon;
};

#endif // TRAYICON_H

