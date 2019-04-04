/*
 * desktop-file-memos: A desktop classification app on Linux.
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Yue Lan<lanyue@kylinos.cn>
 *
 */

#include <QApplication>
#include <QSettings>

#include <QMainWindow>
#include "iconview.h"
#include "splitter.h"
#include "systemtrayicon.h"
#include "settingshelper.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <QSplitter>

#include <QDebug>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define LOCKFILE "/tmp/.desktopfilememos.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static void viewInit();

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename) {
    int fd;
    fd = open(filename, O_RDWR | O_CREAT, LOCKMODE);
    if (lockfile(fd) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    a.setOrganizationName("com.kylin");
    a.setApplicationName("desktopfilememos");
    qDebug()<<a.organizationName();
    qDebug()<<a.applicationName();
    if (already_running(LOCKFILE)) {
        qDebug()<<"isRunning!";
        exit(1);
    }

    viewInit();

    SystemTrayIcon *tray = new SystemTrayIcon;
    tray->show();

    return a.exec();
}

void viewInit() {
    SettingsHelper *settingsHelper = new SettingsHelper;
    QSettings *settings = settingsHelper->getSettings();
    settingsHelper->getSettings()->beginGroup("iconview/id_count");
    int idCout = settingsHelper->getSettings()->value("lastid").toInt();
    settings->endGroup();
    qDebug()<<idCout<<endl;
    for (int i = 1; i <= idCout; i++) {
        settings->beginGroup("iconview/id_title_table");
        QString title_for_this_id = settings->value(QString(i)).toString();
        qDebug()<<settings->value(QString(i));
        qDebug()<<"id: "<<i<<", title: "<<title_for_this_id;
        settings->endGroup();

        settings->beginGroup("iconview/id_state_table");
        int state = settings->value(QString(i)).toInt();
        settings->endGroup();
        qDebug()<<settings->value(QString(i));
        if (state == 1) {
            Splitter *page = new Splitter(i, title_for_this_id);
            page->show();
            page->ensureLastSettings(settings);
        }
    }
    delete settingsHelper;
}
