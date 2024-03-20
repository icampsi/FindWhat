/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "exportEsquema_dlg.h"
#include "ui_exportEsquema_dlg.h"

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"

exportEsquema_dlg::exportEsquema_dlg(QWidget *parent, size_t *pSelectedIndex)
    : QDialog(parent), ui(new Ui::exportEsquema_dlg), m_selectedIndex{pSelectedIndex} {
    ui->setupUi(this);

    for (CEsquemaDoc * esquemaDoc : *CMDoc::getMDoc().getLoadedEsquemaDocs()) {
        ui->comboBox_esquemes->addItem(esquemaDoc->getEsquema()->getName());
    }
}

exportEsquema_dlg::~exportEsquema_dlg() { delete ui; }

void exportEsquema_dlg::on_buttonBox_accepted() {
    if(m_selectedIndex) *m_selectedIndex = ui->comboBox_esquemes->currentIndex();
}
