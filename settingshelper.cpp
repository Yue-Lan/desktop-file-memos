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

#include "settingshelper.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

SettingsHelper::SettingsHelper() {
    mSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
}

void SettingsHelper::resetAll() {
    QSettings *cleanHelper = mSettings;
    cleanHelper->beginGroup("iconview/id_count");
    cleanHelper->clear();
    cleanHelper->endGroup();

    cleanHelper->beginGroup("iconview/id_title_table");
    cleanHelper->clear();
    cleanHelper->endGroup();

    cleanHelper->beginGroup("iconview/id_state_table");
    cleanHelper->clear();
    cleanHelper->endGroup();

    QDir dirHelper;
    /* NOTE:
     * be careful to remove a dir. This will not send the dir to the trash.
     */
    bool isDir = dirHelper.setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.desktop_file_memos");
    if (isDir) {
        dirHelper.removeRecursively();
    }
    dirHelper.setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/com.kylin");
    dirHelper.remove(QString("desktopfilememos.ini"));
    dirHelper.remove(QString("desktopfilememos.conf"));
}

