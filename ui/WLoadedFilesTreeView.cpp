#include "WLoadedFilesTreeView.h"
#include "qfileinfo.h"
#include "qmimedata.h"

WLoadedFilesTreeView::WLoadedFilesTreeView(QWidget* parent) : QTreeView(parent), m_model() {
    setModel(&m_model);
    setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
}

void WLoadedFilesTreeView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Delete) {
        QModelIndexList selIndexes = selectedIndexes();

        // Display confirmation dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Item", "Are you sure you want to delete the selected items?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
            if (model) {
                // Remove selected rows from the model
                for(int i = selIndexes.size() - 1; i >= 0 ; --i) {
                    model->removeRow(selIndexes.at(i).row());
                }
                emitContentOfTreeView();
            }
        }
    }
    else {
        QTreeView::keyPressEvent(event);
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
        for (const QUrl &url : urls) {
            QString path = url.toLocalFile();
            QString fileName = QFileInfo(path).fileName();
            qDebug() << "Dropped file:" << fileName << "Path:" << path;
            // Create a new item with the file name as text and file path as data
            QStandardItem *newItem = new QStandardItem(fileName);
            newItem->setData(path, Qt::UserRole); // Set the file path as data
            m_model.appendRow(newItem);
        }
        event->acceptProposedAction(); // Accept the proposed action (copy)

        // Emit the content of all items in the QTreeView
        emitContentOfTreeView();
    } else {
        // Call base class dropEvent for other types of drops
        QTreeView::dropEvent(event);
    }
}

void WLoadedFilesTreeView::emitContentOfTreeView() {
    std::vector<QString> paths;
    // Get the number of rows
    int rowCount = m_model.rowCount();

    // Iterate through each row
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex index = m_model.index(row, 0);
        QVariant itemData = m_model.data(index, Qt::UserRole);
        paths.push_back(itemData.toString());
    }
    emit contentChanged(paths);
}
