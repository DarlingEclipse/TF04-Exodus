#include "Headers/Main/exUpdater.h"

exUpdater::exUpdater(){
    win_sparkle_set_appcast_url("https://darlingeclipseprogramming.com/Exodus/Exodus_Updates.xml");
    win_sparkle_set_update_check_interval(3600);
    win_sparkle_set_automatic_check_for_updates(1);
    //qDebug() << win_sparkle_get_last_check_time();
    //win_sparkle_check_update_with_ui();
    win_sparkle_init();
}
