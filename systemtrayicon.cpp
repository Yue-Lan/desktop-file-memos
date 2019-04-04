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

#include "systemtrayicon.h"
#include "settingshelper.h"
#include "splitter.h"

#include <QFileIconProvider>
#include <QMenu>
#include <QAction>

#include <QApplication>

#include <QSettings>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

SystemTrayIcon::~SystemTrayIcon() {
    delete mSettingsHelper;
}

SystemTrayIcon::SystemTrayIcon()
{
    mSettingsHelper = new SettingsHelper;
    QSettings *settings = mSettingsHelper->getSettings();

    QFileIconProvider iconProvider;
    setIcon(QIcon(iconProvider.icon(QFileIconProvider::Desktop)));
    QMenu *trayMenu;
    trayMenu = new QMenu();
    QAction *trayQuitAction;
    trayQuitAction = new QAction();
    trayQuitAction->setText(tr("save and quit"));
    QObject::connect(trayQuitAction, &QAction::triggered, [settings](){
        settings->setValue("isRunning",QString("false"));
        qDebug()<<settings->value("isRunning");
        qApp->quit();
    });

    QAction *trayAddWidgetAction = new QAction;
    trayAddWidgetAction->setText(tr("add new"));


    QObject::connect(trayAddWidgetAction, &QAction::triggered, [=](){
        settings->beginGroup("iconview/id_count");
        int lastViewId = settings->value("lastid").toInt();
        //qDebug()<<"lastid: "<<lastViewId;
        //add iconview with mkdir id+1.
        settings->setValue("lastid", lastViewId+1);
        //qDebug()<<settings->value("lastid").toInt();
        settings->endGroup();

        settings->beginGroup("iconview/id_title_table");
        settings->setValue(QString(lastViewId+1), "new"); //title may be changed, but id won't.
        settings->endGroup();

        //TODO: show active window only.
        settings->beginGroup("iconview/id_state_table"); //this is sure whether iconview id is active.
        settings->setValue(QString(lastViewId+1), 1); //'1' is active, '0' is inactive.
        settings->endGroup();

        QDir test;
        test.setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        qDebug()<<test.mkpath(QString(".desktop_file_memos/")+QString::number(lastViewId+1));

        Splitter *newPage = new Splitter(lastViewId+1, QString("new"));
        newPage->show();
    });

    QAction *trayResetAction = new QAction;
    trayResetAction->setText(tr("delete all views and files and quit"));


    QObject::connect(trayResetAction, &QAction::triggered, [=](){
        mSettingsHelper->resetAll();
        settings->setValue("isRunning",QString("false"));
        qDebug()<<settings->value("isRunning");
        qApp->quit();
    });

    trayMenu->addAction(trayAddWidgetAction);
    trayMenu->addAction(trayQuitAction);
    trayMenu->addAction(trayResetAction);
    setContextMenu(trayMenu);

    trayMenu->setAttribute(Qt::WA_TranslucentBackground,true);

    trayMenu->setStyleSheet("background-color:rgba(0,0,0,0.6);"
                            "color:white;");

}
