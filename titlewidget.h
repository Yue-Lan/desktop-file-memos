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

#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>

class QPushButton;
class QSettings;

class TitleWidget : public QWidget {
    Q_OBJECT
public:
    TitleWidget(int id, QString title);
    void setDirPath(QString dirPath) {mDirPath = dirPath;}

    void ensureLastSettings(QSettings *settings);

Q_SIGNALS:
    void viewHideRequest();
    void viewShowRequest();

public Q_SLOTS:
    void onButtonClicked();
    void onWriteFlagRequest();

private:
    int mId = -1;
    QString mTitle = NULL;
    QString mDirPath = NULL;
    bool isPressed = false;
    QPoint clickedPos;

    QPushButton *mHideShowButton;
    bool isViewHiden = false;

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
};

#endif // TITLEWIDGET_H
