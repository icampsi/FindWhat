/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WFormulaTreeView.h"

#include "QCItemDelegateFormulaTreeView.h"

#include <QMenu>
#include <QMessageBox>

WFormulaTreeView::WFormulaTreeView(QWidget *parent) : QTreeView(parent) {
    setStyleSheet("QTreeView::branch {background-color: #A9A9A9;}");
    QCItemDelegateFormulaTreeView *delegate = new QCItemDelegateFormulaTreeView(this);
    setItemDelegate(delegate);

    connect(this, &QTreeView::doubleClicked, this, &WFormulaTreeView::handleDoubleClick);
    connect(delegate, &QCItemDelegateFormulaTreeView::editingFinished, this, &WFormulaTreeView::handleEditingFinished);
}

void WFormulaTreeView::keyPressEvent(QKeyEvent *event) {
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
