/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WESQUEMATREEVIEW_H
#define WESQUEMATREEVIEW_H

#include <QTreeView>
#include <QApplication>
#include <QStandardItem>
#include <QKeyEvent>
#include <QDebug>
#include <QStandardItemModel>

class WEsquemaTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit WEsquemaTreeView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override; // Support "del" key for deleting child elements

signals:
    void itemChanged(const QModelIndex &index, const QString &newText);
    void itemEditingFinished(const QModelIndex &index, const QString &text);
    void removeSecondLevel(const int index, const QModelIndex &parentIndex);

private slots:
    void handleDoubleClick(const QModelIndex &index) {
        if (index.isValid() && index.parent().isValid()) // Only second level items will be able to be eddited
            edit(index);
    }

    void handleEditingFinished(const QModelIndex &index, const QString &text) { emit itemEditingFinished(index, text); }
};

#endif // WESQUEMATREEVIEW_H


