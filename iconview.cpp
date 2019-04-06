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

#include "iconview.h"
#include "filemodel.h"
#include "iconprovider.h"
#include "fileoperationjob.h"
#include "delegate.h"

#include <QFileSystemModel>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>

#include <QMenu>
#include <QAction>

#include <QApplication>
#include <QClipboard>

#include <QMimeData>

#include <QThread>

#include <QDebug>
#include <QLineEdit>

IconView::IconView(int id, QString rootPath) {

    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);

    setStyleSheet("color: white;"
                  "font-size:16px;"
                  "background-color: transparent;"
                  //"border: 1px black;"
                  //"border-radius: 10px;"
                  /*"padding: 2px 4px;"*/);

    QScrollBar *scrollbar = new QScrollBar;
    scrollbar->setStyleSheet("QScrollBar"
                             "{"
                             "background-color:rgba(0,0,0,0.4);"
                             "width:20px;"
                             "border-left:0px;border-right:0px;"
                             "padding-top:20px;"
                             "padding-bottom:20px;"
                             "}"
                             "QScrollBar::add-line:vertical"
                             "{"
                             "height:20px;width:20px;"
                             "subcontrol-position:bottom;"
                             "}"
                             "QScrollBar::sub-line:vertical"
                             "{"
                             "height:20px;width:20px;"
                             "subcontrol-position:top;"
                             "}"
                             "QScrollBar::handle:vertical"
                             "{"
                             "width:20px;"
                             "min-height:50px;"
                             "}");

    setVerticalScrollBar(scrollbar);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //QIcon::setThemeName("ukui-icon-theme");

    mId = id;

    fileModel = new FileModel;
    fileModel->setRootPath(rootPath);
    setModel(fileModel);
    setRootIndex(fileModel->index(rootPath));
    //setRootIndex(fileModel->index("/home/kylin"));

    Delegate *deleget = new Delegate;
    deleget->setFileModel(fileModel);
    deleget->setIconView(this);
    setItemDelegate(deleget);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);
    setDefaultDropAction(Qt::MoveAction);

    int size = fileModel->getIconProvider()->getIconSize();
    setIconSize(QSize(size,size));
    //setUniformItemSizes(true); //same size for all items
    setMovement(QListView::Snap); //gird move
    //setWordWrap(true); //text auto change row
    //setViewportMargins(0,10,0,10);
    //setContentsMargins(31,31,31,31);
    setResizeMode(QListView::Adjust); //auto redo layout
    setGridSize(QSize(size*3,size*3));
    setViewMode(QListView::IconMode);
    //setWordWrap(true);

    setTextElideMode(Qt::ElideMiddle);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    QMenu *viewMenu = new QMenu;
    QAction *openAction = new QAction(viewMenu);
    openAction->setText(tr("open"));
    viewMenu->addAction(openAction);
    //TODO: add open with...
    QAction *selectAllAction = new QAction(viewMenu);
    selectAllAction->setText(tr("select all"));
    viewMenu->addAction(selectAllAction);
    viewMenu->addSeparator();
    QAction *copyAction = new QAction(viewMenu);
    copyAction->setText(tr("copy"));
    viewMenu->addAction(copyAction);
    QAction *pasteAction = new QAction(viewMenu);
    pasteAction->setText(tr("paste"));
    viewMenu->addAction(pasteAction);
    QAction *renameAction = new QAction(viewMenu);
    renameAction->setText(tr("rename"));
    viewMenu->addAction(renameAction);
    QAction *trashAction = new QAction(viewMenu);
    trashAction->setText(tr("trash"));
    viewMenu->addAction(trashAction);
    //TODO: add show propertries action
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &IconView::customContextMenuRequested, [=](){
        //qDebug()<<"menu request"<<mLastPos;
        if (!QApplication::clipboard()->mimeData()->hasUrls()) {
            pasteAction->setEnabled(false);
        } else {
            pasteAction->setEnabled(true);
        }
        if (!indexAt(mLastPos).isValid()) {
            //qDebug()<<"blank";
            clearSelection();
            openAction->setEnabled(false);
            trashAction->setEnabled(false);
            copyAction->setEnabled(false);
            renameAction->setEnabled(false);
        } else {
            //qDebug()<<"selected";
            openAction->setEnabled(true);
            trashAction->setEnabled(true);
            copyAction->setEnabled(true);
            if (selectedIndexes().count() == 1) {
                renameAction->setEnabled(true);
            }
        }
        viewMenu->exec(QCursor::pos());
    });

    connect(openAction, &QAction::triggered, [=](){
       QModelIndexList list = selectedIndexes();
       for (int i = 0; i < list.count(); i++) {
           QUrl url("file://"+fileModel->filePath(list.at(i)));
           QDesktopServices::openUrl(url);
       }
    });

    connect(selectAllAction, &QAction::triggered, [=](){
        selectAll();
    });

    connect(copyAction, &QAction::triggered, [=](){
        QModelIndexList list = this->selectedIndexes();
        QMimeData *selectionMimeData = fileModel->mimeData(list);
        QApplication::clipboard()->setMimeData(selectionMimeData);
    });

    connect(pasteAction, &QAction::triggered, [=]{
        QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
        FileOperationJob *fileOpJob = new FileOperationJob;
        fileOpJob->setOperationFlag(FILE_OPERATION_COPY);
        fileOpJob->setOrigList(urls);
        fileOpJob->setDestDir(fileModel->filePath(rootIndex()));
        QThread *fileOpThread = new QThread;
        fileOpJob->moveToThread(fileOpThread);
        connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
        connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
        connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
        fileOpThread->start();
    });

    connect(renameAction, &QAction::triggered, [=](){
        //this is not good enough
        edit(selectedIndexes().first());
        //TODO use custom delegate for edit;

    });

    connect(trashAction, &QAction::triggered, [=](){
        QModelIndexList list = this->selectedIndexes();
        FileOperationJob *fileOpJob = new FileOperationJob;
        fileOpJob->setOperationFlag(FILE_OPERATION_TRASH);
        fileOpJob->setOrigList(fileModel->mimeData(list)->urls());
        QThread *fileOpThread = new QThread;
        fileOpJob->moveToThread(fileOpThread);
        connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
        connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
        connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
        fileOpThread->start();
    });

    connect(this, &IconView::doubleClicked, [=](){
        if (this->selectedIndexes().count() == 1) {
            QUrl url("file://"+fileModel->filePath(selectedIndexes().at(0)));
            QDesktopServices::openUrl(url);
        }
    });
}

void IconView::paintEvent(QPaintEvent *e) {
    QPainter painter(this->viewport());
    //qDebug()<<"viewport rect: "<<this->viewport()->rect();
    painter.fillRect(this->rect(), QColor(0,0,0,80));
    //qDebug()<<"rect: "<<this->rect();
    return QListView::paintEvent(e);
}

void IconView::mousePressEvent(QMouseEvent *e) {
    mLastPos = e->pos();
    //qDebug()<<"press event: "<<mLastPos;
    QListView::mousePressEvent(e);
}

void IconView::mouseMoveEvent(QMouseEvent *e) {
    setCursor(Qt::ArrowCursor);
    return QListView::mouseMoveEvent(e);
}

void IconView::focusOutEvent(QFocusEvent *e) {
    return QListView::focusOutEvent(e);
}

void IconView::closeEvent(QCloseEvent *e) {
    return QListView::closeEvent(e);
}

void IconView::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()){
        //QList<QUrl> urls = e->mimeData()->urls();
        //qDebug()<<urls<<e->mimeData()->formats();
        if (e->source()==this){
            e->setDropAction(Qt::MoveAction);
            e->accept();
        } else {
            e->acceptProposedAction();
        }
    } else {
        e->ignore();
    }
}

void IconView::dragMoveEvent(QDragMoveEvent *e) {
    if (this == e->source()) {
        return QListView::dragMoveEvent(e);
    }
    e->accept();
}

void IconView::dropEvent(QDropEvent *e) {
    if (this != e->source()) {
        if (e->mimeData()->hasUrls() && !indexAt(e->pos()).isValid()){
            QList<QUrl> urls = e->mimeData()->urls();
            if (!this->indexAt(e->pos()).isValid()) {
                //do the file move job
                FileOperationJob *fileOpJob = new FileOperationJob;
                fileOpJob->setOperationFlag(FILE_OPERATION_MOVE);
                fileOpJob->setOrigList(urls);
                fileOpJob->setDestDir(fileModel->filePath(this->rootIndex()));

                QThread *fileOpThread = new QThread;
                fileOpJob->moveToThread(fileOpThread);
                connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
                connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
                connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
                fileOpThread->start();
            }
        }
    } else {
        e->ignore();
    }
    //return QListView::dropEvent(e);
}
