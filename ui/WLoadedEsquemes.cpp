#include "WLoadedEsquemes.h"
#include "ui_WLoadedEsquemes.h"
#include "QListWidgetItem"
#include <QListWidget>
#include "PMainEsquemaUI.h"
#include "WEsquemaListView.h"

WLoadedEsquemes::WLoadedEsquemes(QWidget *parent)
    : QWidget(parent), ui(new Ui::WLoadedEsquemes) {
    ui->setupUi(this);

    connect(ui->list_esquemes, &WEsquemaListView::deleteEsquema, static_cast<PMainEsquemaUI*>(parent), &PMainEsquemaUI::handleDeleteEsquema);
}


WLoadedEsquemes::~WLoadedEsquemes() { delete ui; }

void WLoadedEsquemes::newEsquema(PEsquemaPage *page, CEsquema *esquema) {
    QListWidgetItem *esquemaItem = new QListWidgetItem(static_cast<QListWidget*>(ui->list_esquemes));
    esquemaItem->setText(esquema->getName());
    m_itemPageMap.insert(esquemaItem, page);

    // Add the item to the QListWidget and select it
    ui->list_esquemes->addItem(esquemaItem);
    ui->list_esquemes->setCurrentItem(esquemaItem);
}

// SLOTS
void WLoadedEsquemes::on_list_esquemes_itemSelectionChanged() {
    PMainEsquemaUI *parent = dynamic_cast<PMainEsquemaUI*>(parentWidget());
    // Check if any esquema is loaded in the ui
    if(!ui->list_esquemes->selectedItems().isEmpty()) {
        QListWidgetItem *item = ui->list_esquemes->selectedItems().front();
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
