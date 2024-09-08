/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WLOADEDFILESTREEVIEW_H
#define WLOADEDFILESTREEVIEW_H

#include <QListView>
#include <QTreeView>
#include <QEvent>
#include <QWidget>



class WLoadedFilesTreeView : public QListView {
    Q_OBJECT
public:
    explicit WLoadedFilesTreeView(QWidget* parent);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
signals:
    void indexChanged(const QModelIndex &current, const QModelIndex &previous);
};
#endif // WLOADEDFILESTREEVIEW_H
