/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WBROWSERTREEVIEW_H
#define WBROWSERTREEVIEW_H

#include <QObject>
#include <QFileSystemModel>
#include <QTreeView>
#include "qevent.h"

class WBrowserTreeView : public QTreeView {
    Q_OBJECT
public:
    WBrowserTreeView(QWidget *parent);

protected:
    // Override the selectionChanged method to emit the filePathChanged signal
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;

    // void dropEvent(QDropEvent *event) override;

signals:
    void filePathChanged(const QString &filePath);

private slots:

};

#endif // WBROWSERTREEVIEW_H


