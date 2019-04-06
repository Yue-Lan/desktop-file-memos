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

#include "iconprovider.h"

#include <QMimeDatabase>
#include <QUrl>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

#include <QDebug>

IconProvider::IconProvider() {
    mimeDatabase = new QMimeDatabase;
}

IconProvider::~IconProvider() {
    delete mimeDatabase;
}

void IconProvider::setIconSize(int size) {
    mIconSize = size;
}

int IconProvider::getIconSize() {
    return mIconSize;
}

QIcon IconProvider::icon(const QFileInfo &info) const {

    QIcon fileIcon;

    //image file, return a thumbnail.
    QPixmap tmpPixmap = QPixmap(info.filePath());
    if (!tmpPixmap.isNull()){
        tmpPixmap = tmpPixmap.scaled(mIconSize, mIconSize);
        fileIcon = QIcon(tmpPixmap);
        return fileIcon;
    }

    //we need do something special to '*.desktop' file.
    if (mimeDatabase->mimeTypeForFile(info).iconName() == QString("application-x-desktop")) {
        std::string tmp_str = info.filePath().toStdString();
        const char* file_path = tmp_str.c_str();
        GDesktopAppInfo *desktop_app_info = g_desktop_app_info_new_from_filename(file_path);
        if (desktop_app_info != nullptr) {
            char* tmp_icon_name = g_desktop_app_info_get_string(desktop_app_info, "Icon");
            QIcon desktopFileIcon = QIcon::fromTheme(QString(tmp_icon_name));
            if (desktopFileIcon.isNull()) {
                QPixmap pixmap = QPixmap(QString (tmp_icon_name));
                if (pixmap.isNull()) {
                    fileIcon = QIcon::fromTheme("application-x-desktop");
                } else {
                    pixmap = pixmap.scaled(mIconSize, mIconSize);
                    QIcon pixmapIcon = QIcon(pixmap);
                    fileIcon = pixmapIcon;
                }
            } else {
                fileIcon = desktopFileIcon;
            }
            if (tmp_icon_name != nullptr) {
                g_free (tmp_icon_name);
            }
        }
        return fileIcon;
    }

    if (mimeDatabase->mimeTypeForUrl(QUrl(info.filePath())).aliases().isEmpty()) {
        return QIcon::fromTheme(mimeDatabase->mimeTypeForFile(info).iconName());
    }
    return QIcon::fromTheme((mimeDatabase->mimeTypeForUrl(QUrl(info.filePath())).iconName()));
}