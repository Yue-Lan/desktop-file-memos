/*
 * splitter.cpp: splitter is an frameless window contain a titlebar and a view.
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

#include "splitter.h"
#include "titlewidget.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSettings>
#include <QPoint>
#include <QCursor>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QSize>
#include <QtX11Extras/QX11Info>
#include <QStandardPaths>

#include <QPainter>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define MARGIN 10
#define MINWIDTH 200
#define MINHEIGHT 150

Splitter::Splitter(int id, QString title) {
    mId = id;
    mTitle = title;

    mLayout = new QVBoxLayout(this);
    mLayout->setSpacing(0);

    mTitleWidget = new TitleWidget(id,title);
    mLayout->addWidget(mTitleWidget);

    mDirPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/.desktop_file_memos/")+QString::number(id);
    mTitleWidget->setDirPath(mDirPath);
    mIconView = new IconView(id, mDirPath);
    mLayout->addWidget(mIconView);

    setMouseTracking(true);
    setMinimumSize(MINWIDTH,MINHEIGHT);
    setContentsMargins(0,0,0,0);

    //set window states
    setWindowFlags (Qt::Window
                    | Qt::FramelessWindowHint
                    | Qt::WindowStaysOnBottomHint
                    | Qt::Tool);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background-color: transparent;"
                  "border: 0 transparent;");

    //follow the current workspace

    Atom net_wm_state_sticky=XInternAtom (QX11Info::display(),
                                          "_NET_WM_STATE_STICKY",
                                          True);

    Atom net_wm_state = XInternAtom (QX11Info::display(),
                                     "_NET_WM_STATE",
                                     False);

    XChangeProperty (QX11Info::display(),
                     winId(),
                     net_wm_state,
                     XA_ATOM,
                     32,
                     PropModeAppend,
                     (unsigned char *)&net_wm_state_sticky,
                     1);

    readSettings();

    connect(this, &Splitter::writeFlagRequest, mTitleWidget, &TitleWidget::onWriteFlagRequest);

    connect(mTitleWidget, &TitleWidget::viewHideRequest, [=](){
        mIconView->hide();
        mLayout->removeWidget(mIconView);
        mLastSize = rect().size();
        //qDebug()<<rect();
        mLayout->removeWidget(mTitleWidget);
        setMinimumHeight(50);
        resize(rect().width(),0);
        Q_EMIT writeFlagRequest();
    });
    connect(mTitleWidget, &TitleWidget::viewShowRequest, [=](){
        mLayout->addWidget(mTitleWidget);
        mLayout->addWidget(mIconView);
        mIconView->show();
        setMinimumHeight(MINHEIGHT);
        resize(mLastSize);
        Q_EMIT writeFlagRequest();

        //Don't comment out this paragraph
        writeSettings();
    });
}

QSize Splitter::getLastWritingSize() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    QRect tmpRect = settings.value("geometry"+QString::number(mId)).toRect();
    return tmpRect.size();
}

void Splitter::ensureLastSettings(QSettings *settings) {
    if (mTitleWidget != NULL) {
        mTitleWidget->ensureLastSettings(settings);
    }
}

void Splitter::readSettings() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    settings.beginGroup("iconview/id_geometry_table");
    //qDebug()<<"read settings";
    //qDebug()<<settings.value("geometry"+QString::number(mId)).toByteArray();
    restoreGeometry(settings.value("geometry"+QString::number(mId)).toByteArray());
    //qDebug()<<settings.value("geometry"+QString::number(mId));
    //qDebug()<<settings.allKeys()<<endl<<settings.childKeys()<<endl<<settings.children();
    settings.endGroup();
}

void Splitter::writeSettings() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    settings.beginGroup("iconview/id_geometry_table");
    settings.setValue("geometry"+QString::number(mId),saveGeometry());
    settings.endGroup();
}

void Splitter::paintEvent(QPaintEvent *e) {
/*
    QPainter painter(this);
    if (mIconView->isVisible())
        painter.fillRect(this->rect(), QColor(0,0,0,80));
*/
    return QWidget::paintEvent(e);
}

void Splitter::closeEvent(QCloseEvent *e) {
    writeSettings();
    QWidget::closeEvent(e);
}

void Splitter::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton)
    {
        this->isLeftPressed = true;
        QPoint temp = e->globalPos();
        pLast = temp;
        mLastRect = geometry();
        //qDebug()<<mLastRect.top()<<" "<<mLastRect.left()<<" "<<mLastRect.bottom()<<" "<<mLastRect.right();
        curPos = countFlag(e->pos(), countRow(e->pos()));

        if(curPos != 22)
        {
            if (mTitleWidget != NULL) {
                mTitleWidget->setEnabled(false);
            }
        }
        //e->ignore();
        e->accept();
    }
    return QWidget::mousePressEvent(e);
}

void Splitter::mouseMoveEvent(QMouseEvent *e) {
    if (!mIconView->isVisible()) {
        e->ignore();
        return QWidget::mouseMoveEvent(e);
    }
    int poss = countFlag(e->pos(), countRow(e->pos()));
    setCursorType(poss);
    if(isLeftPressed)
    {
        if(curPos == 22)
        {

        }
        else
        {
            QRect wid = geometry();
            /*
            qDebug()<<mLastRect<<e->globalPos();
            qDebug()<<"dx:"<<mLastRect.left()-e->globalX();
            qDebug()<<"dy:"<<mLastRect.top()-e->globalY();
            qDebug()<<wid;
            */
            switch(curPos)
            {
            case 11:
                if (true) {
                    wid.setTopLeft(e->globalPos());
                }
                break;
            case 13:
                wid.setTopRight(e->globalPos());
                break;
            case 31:
                wid.setBottomLeft(e->globalPos());
                break;
            case 33:
                wid.setBottomRight(e->globalPos());
                break;
            case 12:
                if (true) {
                    wid.setTop(e->globalY());
                }
                break;
            case 21:
                if (true) {
                    wid.setLeft(e->globalX());
                }
                break;
            case 23:
                if (true) {
                    wid.setRight(e->globalX());
                }
                break;
            case 32:
                wid.setBottom(e->globalY());
                break;
            }
            setGeometry(wid);
            /*
            if (QGuiApplication::screens().constFirst()->availableGeometry().contains(e->pos())) {
                setGeometry(wid);
            }
            */
            e->accept();
        }

        pLast = e->globalPos();
    } else {
        QApplication::restoreOverrideCursor();
    }
    e->ignore();
    QWidget::mouseMoveEvent(e);
}

void Splitter::mouseReleaseEvent(QMouseEvent *) {
    if (mTitleWidget!=NULL) {
        if (mIconView->isVisible()) {
            writeSettings();
        }
        mTitleWidget->setEnabled(true);
    }

    if (mTitleWidget!=NULL) {

    }   
    setCursor(Qt::ArrowCursor);
    isLeftPressed = false;
}

int Splitter::countFlag(QPoint p, int row)
{
    if(p.y() < MARGIN)
        return 10+row;
    else if(p.y() > this->height()-MARGIN)
        return 30+row;
    else
        return 20+row;
}

void Splitter::setCursorType(int flag)
{
    //Qt::CursorShape cursor;
    QCursor cursor;
    switch(flag)
    {
    case 11:
    case 33:
        cursor = Qt::SizeFDiagCursor;
        break;
    case 13:
    case 31:
        cursor = Qt::SizeBDiagCursor;
        break;
    case 21:
    case 23:
        cursor = Qt::SizeHorCursor;
        break;
    case 12:
    case 32:
        cursor = Qt::SizeVerCursor;
        break;
    default:
        QApplication::restoreOverrideCursor();
        break;
    }
    setCursor(cursor);
}

int Splitter::countRow(QPoint p)
{
    return (p.x()<MARGIN) ? 1:( (p.x()>(this->width()-MARGIN) ? 3:2) );
}
