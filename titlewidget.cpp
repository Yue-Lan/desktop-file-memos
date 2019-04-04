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

#include "titlewidget.h"
#include "fileoperationjob.h"
#include <QSettings>

#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

#include <QApplication>
#include <QStyle>

#include <QMenu>
#include <QAction>
#include <QStringList>

#include <QDir>
#include <QThread>
#include <pthread.h>

#include <QDebug>

struct thread_data
{
    QSettings *settings = NULL;
    int id = -1;
    int flag = -1;
};

typedef thread_data settings_data;

void *write_flag_func (void *thread_data) {
    settings_data *data = static_cast<settings_data*>(thread_data);
    QSettings *settings = data->settings;
    int id = data->id;
    int flag = data->flag;
    settings->beginGroup("iconview/id_ishide_table");
    settings->setValue(QString::number(id),flag);
    settings->endGroup();
    delete settings;
    delete data;
    return NULL;
}

void *write_state_func(void *thread_data) {
    settings_data *data = static_cast<settings_data*>(thread_data);
    QSettings *settings = data->settings;
    int id = data->id;
    int flag = data->flag;
    settings->beginGroup("iconview/id_state_table");
    settings->setValue(QString(id),flag);
    settings->endGroup();
    delete settings;
    delete data;
    return NULL;
}

TitleWidget::TitleWidget(int id, QString title) {
    mId = id;
    mTitle = title;
    setFixedHeight(31);

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setEnabled(true);
    titleLabel->setText(title);
    titleLabel->setContentsMargins(31,0,0,0);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleLabel->setAlignment(Qt::AlignHCenter);
    titleLabel->setStyleSheet("font-size:20px;"
                              "color:white;"
                              "background-color:rgba(0,0,0,0.6)");
    titleLabel->setContextMenuPolicy(Qt::CustomContextMenu);

    QMenu *titleWidgetMenu = new QMenu(titleLabel);
    titleWidgetMenu->setAttribute(Qt::WA_TranslucentBackground,true);
    QAction *renameAction = new QAction(titleWidgetMenu);
    renameAction->setText(tr("rename"));
    QAction *restoreAction = new QAction(titleWidgetMenu);
    restoreAction->setText(tr("restore to desltop"));
    QAction *deleteAction = new QAction(titleWidgetMenu);
    deleteAction->setText(tr("delete window"));
    titleWidgetMenu->addAction(renameAction);
    titleWidgetMenu->addSeparator();
    titleWidgetMenu->addAction(restoreAction);
    titleWidgetMenu->addAction(deleteAction);

    mHideShowButton = new QPushButton(this);
    //mHideShowButton->setLayoutDirection(Qt::RightToLeft);
    mHideShowButton->setFixedHeight(31);
    mHideShowButton->setFixedWidth(31);
    //mHideShowButton->setText("Hide");

    mHideShowButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
    mHideShowButton->setStyleSheet("font-size:20px;"
                                   "color:white;"
                                   "background-color:rgba(0,0,0,0.6)");

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(titleLabel);
    hLayout->addWidget(mHideShowButton);

    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->setAlignment(hLayout,Qt::AlignLeading);

    setLayout(hLayout);

    connect(mHideShowButton, &QPushButton::clicked, this, &TitleWidget::onButtonClicked);

    connect(titleLabel, &QLabel::customContextMenuRequested, [=](){
        qDebug()<<"rename menu";
        titleWidgetMenu->exec(QCursor::pos());
    });

    connect(renameAction, &QAction::triggered, [=](){
        QLineEdit *tmpEdit = new QLineEdit;
        tmpEdit->setText(titleLabel->text());
        tmpEdit->setAlignment(Qt::AlignCenter);
        tmpEdit->setStyleSheet("background-color:white;"
                               "font-size:16px;");
        hLayout->replaceWidget(titleLabel, tmpEdit);
        tmpEdit->selectAll();
        tmpEdit->setFocus();
        connect(tmpEdit, &QLineEdit::editingFinished, [=](){
            titleLabel->setText(tmpEdit->text());
            //settings of name ini
            hLayout->replaceWidget(tmpEdit,titleLabel);
            tmpEdit->deleteLater();
        });
    });

    connect(restoreAction, &QAction::triggered, [=](){
        this->parentWidget()->hide();
        QStringList fakeDirStringList;

        QDir dir(mDirPath);
        dir.cdUp();
        QString tmpDirPath = dir.path()+"/memos"+QString::number(mId)+"_"+titleLabel->text();
        qDebug()<<tmpDirPath;
        qDebug()<<dir.rename(mDirPath, tmpDirPath);
        fakeDirStringList.append(tmpDirPath);
        FileOperationJob *fileOpJob = new FileOperationJob;
        fileOpJob->setOperationFlag(FILE_OPERATION_SENDTO_DESKTOP);
        fileOpJob->setOrigList(fakeDirStringList);
        QThread *fileOpThread = new QThread;
        fileOpJob->moveToThread(fileOpThread);
        connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
        connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
        connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
        fileOpThread->start();

        pthread_t t1;
        settings_data *data = new settings_data;
        data->id = mId;

        QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
        data->settings = settings;
        data->flag = -1;
        qDebug()<<data->id<<data->flag;
        pthread_create(&t1, NULL, write_state_func, static_cast<void*>(data));
    });

    connect(deleteAction, &QAction::triggered, [=](){
        this->parentWidget()->hide();
        //rename the dir from title and trash it.
        QStringList fakeDirStringList;

        QDir dir(mDirPath);
        dir.cdUp();
        QString tmpDirPath = dir.path()+"/memos"+QString::number(mId)+"_"+titleLabel->text();
        qDebug()<<tmpDirPath;
        qDebug()<<dir.rename(mDirPath, tmpDirPath);
        fakeDirStringList.append(tmpDirPath);
        FileOperationJob *fileOpJob = new FileOperationJob;
        fileOpJob->setOperationFlag(FILE_OPERATION_SENDTO_DESKTOP_AND_TRASH);
        fileOpJob->setOrigList(fakeDirStringList);
        QThread *fileOpThread = new QThread;
        fileOpJob->moveToThread(fileOpThread);
        connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
        connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
        connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
        fileOpThread->start();

        pthread_t t1;
        settings_data *data = new settings_data;
        data->id = mId;

        QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
        data->settings = settings;
        data->flag = -1;
        qDebug()<<data->id<<data->flag;
        pthread_create(&t1, NULL, write_state_func, static_cast<void*>(data));
    });

}

void TitleWidget::ensureLastSettings(QSettings *settings) {
    settings->beginGroup("iconview/id_ishide_table");
    int flag = settings->value(QString::number(mId)).toInt();
    settings->endGroup();
    qDebug()<<flag;
    if (flag == 1) {
        mHideShowButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
        isViewHiden = true;
        Q_EMIT viewHideRequest();
    }
}

void TitleWidget::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        setCursor(Qt::SizeAllCursor);
    }
    isPressed = true;
    clickedPos = e->pos();
    //qDebug()<<clickedPos;
    QWidget::mousePressEvent(e);
}

void TitleWidget::mouseMoveEvent(QMouseEvent *e) {
    if (isPressed) {
        if (parentWidget() != NULL)
            parentWidget()->move(e->globalPos()-clickedPos);
    }
    QWidget::mouseMoveEvent(e);
}

void TitleWidget::mouseReleaseEvent(QMouseEvent *e) {
    setCursor(Qt::ArrowCursor);

    //We must ensure that the program can save the state last shutdown
    //when it is started at this time. This may not be elegant enough.
    //Maybe instead of using the restoregeometry() method,
    //using move() and resize() could make the logic clearer.
    if (isViewHiden && isPressed) {
        Q_EMIT viewShowRequest();
        Q_EMIT viewHideRequest();
    }
    isPressed = false;
    QWidget::mouseReleaseEvent(e);
}

void TitleWidget::onButtonClicked() {

    if (!isViewHiden) {
        //mHideShowButton->setText("Show");
        mHideShowButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
        isViewHiden = true;
        Q_EMIT viewHideRequest();
    } else {
        //mHideShowButton->setText("Hide");
        mHideShowButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        isViewHiden = false;
        Q_EMIT viewShowRequest();
    }
}

void TitleWidget::onWriteFlagRequest() {
    //move this to thread
    pthread_t t1;
    settings_data *data = new settings_data;
    data->id = mId;

    QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    data->settings = settings;
    if (isViewHiden) {
        data->flag = 1;
    } else {
        data->flag = 2;
    }
    qDebug()<<data->id<<data->flag;
    pthread_create(&t1, NULL, write_flag_func, static_cast<void*>(data));
}
