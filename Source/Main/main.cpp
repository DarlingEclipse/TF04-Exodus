#include "Headers/Main/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProgWindow w;
    w.setWindowTitle("Exodus 0.7.4.0");
    w.setGeometry(QRect(QPoint(0,0), QSize(w.hSize,w.vSize)));
    //w.resize(1200, 600);
    w.show();
    win_sparkle_set_appcast_url("https://darlingeclipseprogramming.com/Exodus/Exodus_Updates.xml");
    win_sparkle_set_update_check_interval(3600);
    win_sparkle_set_automatic_check_for_updates(1);
    //qDebug() << win_sparkle_get_last_check_time();
    //win_sparkle_check_update_with_ui();
    win_sparkle_init();
    return a.exec();
}
