#include "login.h"
#include <QLockFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLockFile lockFile(QDir::tempPath()+"/locker_base.lock");

    if(!lockFile.tryLock(100)){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("El programa ya se encuentra en ejecución");
        msgBox.exec();
        return 1;
    }

    Login w;

    QIcon icon(":/images/LPL.ico");
    w.setWindowIcon(icon);

    w.show();
    return a.exec();
}
