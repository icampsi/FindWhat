/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WLOADEDFILESTREEVIEW_H
#define WLOADEDFILESTREEVIEW_H

#include <QTreeView>
#include <QEvent>
#include <QStandardItemModel>
#include <QWidget>

class WLoadedFilesTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit WLoadedFilesTreeView(QWidget* parent);

signals:
    void contentChanged(const std::vector<QString>& paths);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void emitContentOfTreeView();

protected:
    QStandardItemModel m_model;
};
#endif // WLOADEDFILESTREEVIEW_H
