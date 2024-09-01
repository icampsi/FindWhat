/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PFormExpToolBoxPage.h"

#include <QComboBox>

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"

#ifdef ENABLE_DBMANAGER
#include "WSqlMultiTable.h"
#include "CSqlMultiTableDelegate.h"
#endif

PFormExpToolBoxPage::PFormExpToolBoxPage(QWidget *parent, CExportCSV *exportCSV)
    : QWidget(parent), ui(new Ui::PExpFormToolBoxPage), m_exportCSV{exportCSV}
{
    // BOOKMARK - Right now this constructor is used whether we are creating a new page or loading it. Maybe I should create one for each of those cases.
    ui->setupUi(this);
    ui->lineEdit_renameDocs->setEnabled(false);

    CMDoc &cmdoc = CMDoc::getMDoc();
    // Initialize m_exportCSV
    if(!m_exportCSV) m_exportCSV = cmdoc.getExportPathDoc().createExportCSV();

    // Get all loaded esquemes for initial comboBox setup
    const std::vector<CEsquemaDoc*> *esquemadocs = cmdoc.getLoadedEsquemaDocs();

    {
        QString assocEsquemaName("");
        if(exportCSV) {
            assocEsquemaName = exportCSV->getAsocEsquema()->getName();
            updateFields();
        }
        // Add Esquema names to the combo box
        for (const CEsquemaDoc *esquemaDoc : *esquemadocs) {
            ui->comboBox_esquemaName->addItem(esquemaDoc->getEsquema()->getName());
        }
        if(!assocEsquemaName.isEmpty()) { // Update esquema name if there was any
            ui->comboBox_esquemaName->setCurrentText(assocEsquemaName);
        }
    }

    /* Set up observer for changes in the esquema list,
     * so the combobox can display the correct esquemes in the correct order for axessing them later simply through its index */
    m_observerHandle.push_back(cmdoc.addObserver(std::bind(&PFormExpToolBoxPage::onEsquemaListChanged, this, std::placeholders::_1)));

    connect(ui->TreeList_files, &WLoadedFilesTreeView::contentChanged, this, &PFormExpToolBoxPage::handlePathContentChanged);

    // If there are loaded Esquema documents, associate the first one with the export CSV
    if (!esquemadocs->empty() && m_exportCSV->getAsocEsquema() == nullptr) {
        m_exportCSV->setAsocEsquema(esquemadocs->front()->getEsquema());
    }
    
    ui->spreadSheet_formatTable->setModel(m_exportCSV->getCsvTableModel());
    // m_exportCSV->getDbTableModel()->updateFields(CDbConnection::getConnection().getModel("Utility Bills")); new BOOKMARK

#ifdef ENABLE_DBMANAGER
    // Create CRecTable for sql insertions. Used if parseDB is selected
    WSqlMultiTable *dbTable = new WSqlMultiTable(ui->stackedWidget_parseOpt);

    // Disable custom widgets in order to only get line edits on every cell.
    CSqlMultiTableDelegate* dbTableDelegate = qobject_cast<CSqlMultiTableDelegate*>(dbTable->itemDelegate());
    dbTableDelegate->disableCustomWidgets(true);

    // Set the recModel generated and sotred by the exportCSV as the table model.
    dbTable->setSqlRecordModel(m_exportCSV->getDbTableModel());

    // Insert table into page 1 of stack widget.
    ui->stackedWidget_parseOpt->insertWidget(1, dbTable);
    ui->stackedWidget_parseOpt->setCurrentWidget(dbTable);
#endif
}

PFormExpToolBoxPage::~PFormExpToolBoxPage() {
    // Remove observers
    for(size_t& handle : m_observerHandle) {
        CMDoc::getMDoc().removeObserver(handle);
    }
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

void PFormExpToolBoxPage::updateFields() {
    // Esquema Name
    ui->comboBox_esquemaName->setCurrentText(m_exportCSV->getAsocEsquema()->getName());
    // Rename Checkbox
    ui->checkBox_renameDocs->setChecked(m_exportCSV->getRenameParsedPDFFlag());
    // Act only if textBox
    ui->lineEdit_actOnlyIf->setText(m_exportCSV->getIdText());
    // Rename text formula
    ui->lineEdit_renameDocs->setText(m_exportCSV->getFileNamePlaceholder());
}

void PFormExpToolBoxPage::on_comboBox_esquemaName_currentIndexChanged(int index) { // Sets the associated esquema to the one that has the same index in loadedEsquemaDocs
    if (index < 0 || static_cast<size_t>(index) > CMDoc::getMDoc().getLoadedEsquemaDocs()->size()) {
        m_exportCSV->setAsocEsquema(nullptr);
    } else {
        CEsquema *esquema = CMDoc::getMDoc().getLoadedEsquemaDocs()->at(index)->getEsquema();
        m_exportCSV->setAsocEsquema(esquema);
    }
}

void PFormExpToolBoxPage::on_checkBox_renameDocs_stateChanged(int arg1) {
    if (arg1) {
        m_exportCSV->setRenameParsedPDFFlag(true); // Set flag for renaming
        ui->lineEdit_renameDocs->setEnabled(true);
    } else {
        m_exportCSV->setRenameParsedPDFFlag(false); // Set flag for renaming
        ui->lineEdit_renameDocs->setEnabled(false);
    }
}
