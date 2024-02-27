#include "WLoadedEsquemes.h"
#include "ui_WLoadedEsquemes.h"
#include "QListWidgetItem"
#include "PMainEsquemaUI.h"

WLoadedEsquemes::WLoadedEsquemes(QWidget *parent)
    : QWidget(parent), ui(new Ui::WListWidgetLoadedEsquemes)
{
    ui->setupUi(this);
}

WLoadedEsquemes::~WLoadedEsquemes() { delete ui; }

void WLoadedEsquemes::newEsquema(PEsquemaPage *page, CEsquema *esquema) {
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
    if(item) {
        PMainEsquemaUI *parent = dynamic_cast<PMainEsquemaUI*>(parentWidget());
        if(parent) {
            parent->changeCurrentPage(m_itemPageMap[item]);
        } else qDebug() << "No item selected in list_esquemes";
    } else qDebug() << "Selected item not found in m_itemPageMap";
}
