/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WLoadedFilesTreeView.h"
#include "CParsedFileModel.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QMimeData>
#include <QFileInfo>
#include "dialogs/ProgBar_dlg.h"
#include "qcoreapplication.h"

WLoadedFilesTreeView::WLoadedFilesTreeView(QWidget* parent) : QListView(parent) {
    setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
}

void WLoadedFilesTreeView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Delete) {

        // Display confirmation dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Item", "Are you sure you want to delete the selected items?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            CParsedFileModel *model = dynamic_cast<CParsedFileModel*>(this->model());
            if (model) {
                // Remove selected rows from the model
                QModelIndexList selIndexes = selectedIndexes();
                model->removeData(selIndexes);
            }
        }
    }
    else {
        QListView::keyPressEvent(event);
    }
}

void WLoadedFilesTreeView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            QString path = url.toLocalFile();
            QString fileExtension = QFileInfo(path).suffix();
            if(fileExtension != "pdf") {
                return;
            }
        }
        event->acceptProposedAction();
    }
}

void WLoadedFilesTreeView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void WLoadedFilesTreeView::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        // Show progress bar dlg
        ProgBar_dlg *pb_dlg = new ProgBar_dlg(urls.size(), "Parsing files...", nullptr);
        pb_dlg->show();
        QCoreApplication::processEvents(); // Needed to display progress bar

        // Append filePaths to model
        for (const QUrl &url : urls) {
            QString path = url.toLocalFile();
            QString fileName = QFileInfo(path).fileName();
            qDebug() << "\nDropped file:" << fileName << "\nPath:" << path;
            // Drop data tot he model
            static_cast<CParsedFileModel*>(model())->appendData(path);
            pb_dlg->updateProgress();
            QCoreApplication::processEvents(); // Needed to display progress bar
        }
        delete pb_dlg;
        event->acceptProposedAction(); // Accept the proposed action (copy)
    } else {
        // Call base class dropEvent for other types of drops
        QListView::dropEvent(event);
    }
}

void WLoadedFilesTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    QListView::currentChanged(current, previous);
    emit indexChanged(current, previous);
}
