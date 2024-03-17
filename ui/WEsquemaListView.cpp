/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WEsquemaListView.h"

#include <QMessageBox>

WEsquemaListView::WEsquemaListView(QWidget *parent) : QListWidget{parent} {}

void WEsquemaListView::keyPressEvent(QKeyEvent *event) {
    //REMOVE ITEMS
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        QModelIndex selectedIndex = currentIndex();
        // Retrieve the item's data
        QVariant itemData = model()->data(selectedIndex);
        QString  itemName = itemData.toString();

        // Display confirmation dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Esquema", "Are you sure you want to delete '" + itemName + "'?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if(count() == 0) return; // if there are no functions, return
            const int index = currentRow(); // Get the index of the current row

            // Ensures a valid row is selected
            if (index != -1) {
                emit deleteEsquema(index);
            }
            else QMessageBox::warning(this, "No Esquema Selected", "Please select a row to delete.");
        }
    }
    else QListView::keyPressEvent(event);
}
