#include "WLoadedEsquemes.h"
#include "ui_WLoadedEsquemes.h"
#include "QListWidgetItem"
#include "WMainEsquemaUI.h"

WLoadedEsquemes::WLoadedEsquemes(QWidget *parent)
    : QWidget(parent), ui(new Ui::WListWidgetLoadedEsquemes)
{
    ui->setupUi(this);
}

WLoadedEsquemes::~WLoadedEsquemes() { delete ui; }

void WLoadedEsquemes::newEsquema(EsquemaPage *page, CEsquema *esquema) {
    QListWidgetItem *esquemaItem = new QListWidgetItem(ui->list_esquemes);
    esquemaItem->setText(esquema->getName());
    m_itemPageMap.insert(esquemaItem, page);

    // Add the item to the QListWidget and select it
    ui->list_esquemes->addItem(esquemaItem);
    ui->list_esquemes->setCurrentItem(esquemaItem);
}

// SLOTS
void WLoadedEsquemes::on_list_esquemes_itemSelectionChanged() {
    QListWidgetItem *item = ui->list_esquemes->selectedItems().front();
    dynamic_cast<WMainEsquemaUI*>(parentWidget())->changeCurrentPage(m_itemPageMap[item]);
}

