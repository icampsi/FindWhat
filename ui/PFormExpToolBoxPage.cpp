/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PFormExpToolBoxPage.h"
#include "ui_PFormExpToolBoxPage.h"

#include "QComboBox"

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"

PFormExpToolBoxPage::PFormExpToolBoxPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::PExpFormToolBoxPage)
{
    ui->setupUi(this);

    // Initiallize m_exportCSV
    m_exportCSV = CMDoc::getMDoc().getExportPathDoc().createExportCSV();
    // Set up observer for changes in the esquema list,
    // so the combobox can display the correct esquemes in the correct order for axessing them later simply through its index
    CMDoc::getMDoc().addObserver(std::bind(&PFormExpToolBoxPage::onEsquemaListChanged, this, std::placeholders::_1));
    connect(ui->TreeList_files, &WLoadedFilesTreeView::contentChanged, this, &PFormExpToolBoxPage::handlePathContentChanged);

    // Get all loaded esquemes for initial comboBox setup
    const std::vector<CEsquemaDoc*> *esquemadocs = CMDoc::getMDoc().getLoadedEsquemaDocs();

    // If there are loaded Esquema documents, associate the first one with the export CSV
    if (!esquemadocs->empty()) {
        m_exportCSV->setAsocEsquemaDoc(esquemadocs->front());
    }

    // Add Esquema names to the combo box
    for (const auto esquemaDoc : *esquemadocs) {
        ui->comboBox_esquemaName->addItem(esquemaDoc->getEsquema()->getName());
    }

    ui->lineEdit_renameDocs->setEnabled(false);
}

PFormExpToolBoxPage::~PFormExpToolBoxPage() {
    delete ui;
}

void PFormExpToolBoxPage::onEsquemaListChanged(const std::vector<QString>& updatedEsquemaDocList) {
    // Delete current items
    ui->comboBox_esquemaName->clear();

    // Refill the esquema list
    for(auto& it : updatedEsquemaDocList) {
        ui->comboBox_esquemaName->addItem(it);
    }
}

void PFormExpToolBoxPage::on_comboBox_esquemaName_currentIndexChanged(int index) { // Sets the associated esquema to the one that has the same index in loadedEsquemaDocs
    if (index < 0 || static_cast<size_t>(index) > CMDoc::getMDoc().getLoadedEsquemaDocs()->size()) {
        m_exportCSV->setAsocEsquemaDoc(nullptr);
    }
    else {
        CEsquemaDoc *esquemaDoc = CMDoc::getMDoc().getLoadedEsquemaDocs()->at(index);
        m_exportCSV->setAsocEsquemaDoc(esquemaDoc);
    }
}

void PFormExpToolBoxPage::on_checkBox_renameDocs_stateChanged(int arg1) {
    if (arg1) {
        m_exportCSV->setRenameParsedPDFFlag(true); // Set flag for renaming
        ui->lineEdit_renameDocs->setEnabled(true);
    }
    else {
        m_exportCSV->setRenameParsedPDFFlag(false); // Set flag for renaming
        ui->lineEdit_renameDocs->setEnabled(false);
    }
}


void PFormExpToolBoxPage::on_lineEdit_renameDocs_textChanged(const QString &arg1) {
    m_exportCSV->setFileNamePlaceholder(arg1);
}

