#include "WExtendedListWidget.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QEvent>
#include <QMenu>

WExtendedListWidget::WExtendedListWidget(QWidget *parent) : QListWidget{parent} {}

void WExtendedListWidget::keyPressEvent(QKeyEvent *event) {
    //REMOVE ITEMS
    if (event->key() == Qt::Key_Delete) {
        QListWidgetItem *item = currentItem();
        if (item) {
            emit itemDeletitionRequested(currentRow());
        }
    }
    else {
        QListWidget::keyPressEvent(event); // Call base class implementation for other keys
    }
}

void WExtendedListWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QListWidgetItem *item = itemAt(event->pos());
        QModelIndex index = indexAt(event->pos()); // Get the index of the clicked item
        QMenu contextMenu(this);

        if (item) {
            QAction *action_Rename = new QAction("Rename", this);
            QAction *action_Delete = new QAction("Delete", this);

            connect(action_Rename, &QAction::triggered, this, [=]() {renameItem(index);});
            connect(action_Delete, &QAction::triggered, this, [=]() { emit itemDeletitionRequested(index.row());});

            contextMenu.addAction(action_Rename);
            contextMenu.addAction(action_Delete);

        } else {
            QAction *action_addNew = new QAction("Add New", this);
            connect(action_addNew, &QAction::triggered, this, [=]() { emit addNewItemRequested(); });
            contextMenu.addAction(action_addNew); // BOOKMARK - Needs implementation
        }
        QPoint globalPos = mapToGlobal(event->pos()); // Convert local position to global
        contextMenu.exec(globalPos); // Use global position for menu
    }
    QListWidget::mousePressEvent(event);
}
