#include "dockspreadsheet.h"
#include "ui_dockspreadsheet.h"

dockSpreadSheet::dockSpreadSheet(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::dockSpreadSheet)
{
    ui->setupUi(this);
}

dockSpreadSheet::~dockSpreadSheet() {
    delete ui;
}
