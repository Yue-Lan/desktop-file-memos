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

#include "delegate.h"
#include "fileoperationjob.h"

#include <QTextEdit>
#include <QStaticText>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QThread>
#include <QDebug>

Delegate::Delegate() {

}

QWidget *Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    /*
    QLabel *lable = new QLabel(parent);
    lable->setText(index.data().toString());
    lable->setFixedWidth(108);
    lable->setStyleSheet("background-color:white;"
                         "color:black;");
    lable->setWordWrap(true);
    //lable->resize(option.rect.size());
    qDebug()<<option.rect;
    lable->adjustSize();
    qDebug()<<lable->rect();
    lable->setFixedHeight(200);
    return lable;
    */
    //this is not good enough.
    QTextEdit *textEdit = new QTextEdit(parent);
    //textEdit->setWordWrapMode();
    //textEdit->setAttribute(Qt::WA_TranslucentBackground,false);
    textEdit->setStyleSheet("background-color:white;"
                            "color:black;");
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setAlignment(Qt::AlignHCenter);
    textEdit->setText(index.data().toString());
    textEdit->setFixedWidth(option.rect.width());
    textEdit->setFixedHeight(150);
    //textEdit->adjustSize();
    /*
    QStaticText text;
    text.setText(index.data().toString());
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::WordWrap);
    text.setTextOption(textOption);
    text.setTextWidth(108);
    //text.prepare();
    */
    //qDebug()<<text.size();
    //qDebug()<<textEdit->rect()<<option.rect;
    connect(textEdit, &QTextEdit::destroyed, [=](){
        //qDebug()<<"textEdit destroyed";
        //qDebug()<<index.data();
        //qDebug()<<textEdit->toPlainText();
        //qDebug()<<mModel->filePath(mIconView->rootIndex());
        if (!textEdit->toPlainText().isEmpty()) {
            QString srcPath = mModel->filePath(mIconView->rootIndex())+"/"+index.data().toString();
            QString destPath = mModel->filePath(mIconView->rootIndex())+"/"+textEdit->toPlainText();
            QFileInfo info(srcPath);
            if (info.isDir()) {
                QDir dir;
                dir.rename(srcPath,destPath);
            } else {
                QFile file;
                file.rename(srcPath,destPath);
            }
        }
    });
    return textEdit;
}

void Delegate::setEditorData(QWidget *, const QModelIndex &) const {
    return;
}

void Delegate::setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const {
    return;
}

void Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QStyledItemDelegate::updateEditorGeometry(editor,option,index);
}