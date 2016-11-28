#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>
#include <QDesktopServices>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

private slots:
    void on_textBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
