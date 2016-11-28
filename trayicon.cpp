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

    setIcon(QIcon(":/icon/icon_trayIcon"));   //����ͼ��ͼƬ
    setToolTip("WebControl");             //��ͼƬ���õ�������
    setContextMenu(menuTrayIcon);
    show();
}
