#include "trayicon.h"

TrayIcon::TrayIcon() : QSystemTrayIcon()
{

}
TrayIcon::TrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
{

    menuTrayIcon = new QMenu((QWidget*)QApplication::desktop());


    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    setIcon(QIcon(":/icon/icon_trayIcon"));   //设置图标图片
    setToolTip("WebControl");             //把图片设置到窗口上
    setContextMenu(menuTrayIcon);
    show();
}
