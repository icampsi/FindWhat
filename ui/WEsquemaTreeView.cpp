// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "WEsquemaTreeView.h"

#include "QCItemDelegateFormulaTreeView.h"
#include <QMessageBox>

WEsquemaTreeView::WEsquemaTreeView(QWidget *parent) : QTreeView(parent) {
    setStyleSheet("QTreeView::branch {background-color: #A9A9A9;}");
    QCItemDelegateFormulaTreeView *delegate = new QCItemDelegateFormulaTreeView(this); // Assuming EsquemaPage is a QObject
    setItemDelegate(delegate);

    connect(this, &QTreeView::doubleClicked, this, &WEsquemaTreeView::handleDoubleClick);
    connect(delegate, &QCItemDelegateFormulaTreeView::editingFinished, this, &WEsquemaTreeView::handleEditingFinished);
}

void WEsquemaTreeView::keyPressEvent(QKeyEvent *event) {
    //REMOVE ITEMS
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        QModelIndex selectedIndex = currentIndex();
        if (selectedIndex.isValid() && selectedIndex.parent().isValid()) {
            // Retrieve the item's data
            QVariant itemData = model()->data(selectedIndex);
            QString  itemName = itemData.toString();

            // Display confirmation dialog
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete Item", "Are you sure you want to delete '" + itemName + "'?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
                if (model) {
                    // Emit rowsRemoved signal
                    emit removeSecondLevel(selectedIndex.row(), selectedIndex.parent());
                }
            }
        }
    } else {
        QTreeView::keyPressEvent(event);
    }
}

void WEsquemaTreeView::handleDoubleClick(const QModelIndex &index) {
    // Only second level items will be able to be eddited
    if (index.isValid() && index.parent().isValid())
        edit(index);
}

void WEsquemaTreeView::handleEditingFinished(const QModelIndex &index, const QString &text) {
    // Emit a new signal to send the information further
    emit itemEditingFinished(index, text);
}
