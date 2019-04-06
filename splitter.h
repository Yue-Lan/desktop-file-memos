/*
 * splitter.h: splitter is an frameless window which contains a titlebar
 * and a view.
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

#ifndef SPLITTER_H
#define SPLITTER_H

/*
 * Splitter is an frameless window still at the bottom, which contain
 * a titlebar and an iconview. It follows the current workspace.
 * Drag and zoom functions must be re-implemented in splitter.
 * The function of window movement is realized by titlebar,
 * and splitter realize the scale function by overiding mouse event.
 *
 * It aslo need be enable to restore from last time application runned
 * if the applition started again. It is convenient to use qsettings
 * to achieve this function.
 *
 * Not only the window state but also the iconview state should
 * be restored. There's going to be some complicated. State of icon
 * view is not splitter's business.
 */

#include <QWidget>
#include "titlewidget.h"
#include "iconview.h"

class QVBoxLayout;
class QSettings;

class Splitter : public QWidget {
    Q_OBJECT
public:
    Splitter(int id, QString title);

    QSize getLastWritingSize();
    void readSettings();
    void writeSettings();
    QString getDirPath(){return mDirPath;}

    void ensureLastSettings(QSettings *settings);

    TitleWidget *getTitleWidget () {return mTitleWidget;}
    IconView *getIconView () {return mIconView;}

    int countFlag(QPoint p, int row);
    void setCursorType(int flag);
    int countRow(QPoint p);

Q_SIGNALS:
    void writeFlagRequest();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    void closeEvent(QCloseEvent *e) override;
    bool isLeftPressed = false;
    int curPos;
    QPoint pLast;
    QRect mLastRect;
    QSize mLastSize;

    //TitleBar *mTitleBar = nullptr;
    int mId = -1;
    QString mTitle = nullptr;
    QString mDirPath = nullptr;

    QVBoxLayout *mLayout = nullptr;
    TitleWidget *mTitleWidget = nullptr;
    IconView *mIconView = nullptr;
};

#endif // SPLITTER_H
