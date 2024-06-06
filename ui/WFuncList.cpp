#include "WFuncList.h"
#include "src/CEsquema.h"
#include "ui/PMainEsquemaUI.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QEvent>
#include <QMenu>

WFuncList::WFuncList(QWidget *parent) : QListWidget{parent} {
    connect(this, &WFuncList::deleteEsquema, static_cast<PMainEsquemaUI*>(parent), &PMainEsquemaUI::handleDeleteEsquema);
    connect(this, &QListWidget::itemSelectionChanged, this, &WFuncList::handleItemSelectionChanged);
    connect(this, &QListWidget::itemChanged, this, &WFuncList::handleItemChanged);
}

void WFuncList::keyPressEvent(QKeyEvent *event) {
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

void WFuncList::newEsquema(PEsquemaPage *page, CEsquema *esquema) {
    QListWidgetItem *esquemaItem = new QListWidgetItem(static_cast<QListWidget*>(this));
    esquemaItem->setFlags(esquemaItem->flags() | Qt::ItemIsEditable);
    esquemaItem->setText(esquema->getName());
    m_itemPageMap.insert(esquemaItem, page);

    // Add the item to the QListWidget and select it
    addItem(esquemaItem);
    setCurrentItem(esquemaItem);
}

void WFuncList::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QListWidgetItem *item = itemAt(event->pos());
        QModelIndex index = indexAt(event->pos()); // Get the index of the clicked item

        if (item) {
            QMenu contextMenu(this);
            QAction *action_Rename = new QAction("Rename", this);
            QAction *action_Delete = new QAction("Delete", this);

            connect(action_Rename, &QAction::triggered, this, [=]() {renameItem(index);}); // BOOKMARK - Needs implementation
            connect(action_Delete, &QAction::triggered, this, [=]() { emit deleteEsquema(index.row());});

            contextMenu.addAction(action_Rename);
            contextMenu.addAction(action_Delete);

            QPoint globalPos = mapToGlobal(event->pos()); // Convert local position to global
            contextMenu.exec(globalPos); // Use global position for menu
        }
    }
    QListWidget::mousePressEvent(event);
}

// SLOTS
void WFuncList::handleItemChanged(QListWidgetItem *item) {
    Q_UNUSED(item);
    // int index = indexFromItem(item).row();
    // QString newName = item->text();

    // if(!newName.isEmpty())
    //    CMDoc::getMDoc().getEsquemaFromIndex(index)->getEsquema()->setName(newName); // BOOKMARK - this line is completly wrong and the full code for renaming functions is broken
}

void WFuncList::handleItemSelectionChanged() {
    PMainEsquemaUI *parent = dynamic_cast<PMainEsquemaUI*>(parentWidget());
    // Check if any esquema is loaded in the ui
    if(!selectedItems().isEmpty()) {
        QListWidgetItem *item = selectedItems().front();
        if(item) {
            if(parent) {
                parent->changeCurrentPage(m_itemPageMap[item]);
            } else qDebug() << "No item selected in list_esquemes";
        } else qDebug() << "Selected item not found in m_itemPageMap";
    }
    else { // Else, set the esquema page to m_emptyPage of PMainEsquemaUI
        if (parent) {
            parent->setCurrentPageToEmptyPage();
        }
    }
}
