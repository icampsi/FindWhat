// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "WBrowserTreeView.h"
#include "qmimedata.h"
#include "utils/generalfunctions.h"

WBrowserTreeView::WBrowserTreeView(QWidget *parent) : QTreeView(parent) {
    setAcceptDrops(true);

    // Create a model for the file system
    QFileSystemModel* model = new QFileSystemModel;
    model->setRootPath(QDir::rootPath());  // Set the root path to the root of the file system

    // Set a name filter to include only PDF files
    model->setNameFilters(QStringList() << "*.pdf");
    model->setNameFilterDisables(false);  // Enable the name filter

    // Set the model for the browser
    setModel(model);
    setRootIndex(model->index(getUserHomeDirectory()));  // Set the root index to the root path
    setColumnHidden(1, true);  // Hide Size column
    setColumnHidden(2, true);  // Hide Type column
    setColumnHidden(3, true);  // Hide modified column
}

void WBrowserTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)  {
    QTreeView::selectionChanged(selected, deselected);
    QModelIndexList indexes = selected.indexes();
    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(this->model());
        if (model) {
            QString filePath = model->filePath(index);
            QFileInfo fileInfo(filePath);
            if (fileInfo.suffix().toLower() == "pdf") {
                emit filePathChanged(filePath);
            }
        }
    }
}

void WBrowserTreeView::dragEnterEvent(QDragEnterEvent *event)  {
    if (event->mimeData()->hasUrls()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void WBrowserTreeView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

// void WBrowserTreeView::dropEvent(QDropEvent *event)  {
//     if (event->mimeData()->hasUrls()) {
//         QList<QUrl> urls = event->mimeData()->urls();
//         for (const QUrl &url : urls) {
//             addDroppedItem(url);
//         }
//         event->accept();
//     } else {
//         event->ignore();
//     }
// }
