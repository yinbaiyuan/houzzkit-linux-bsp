#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font;
    QRect availableGeometry = QApplication::desktop()->availableGeometry();
    font.setPixelSize(availableGeometry.height()/40);
    a.setFont(font);
    MainWindow w;
    w.setWindowModality(Qt::ApplicationModal);
    w.showMaximized();
    return a.exec();
}
