/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PFormExpToolBoxPage.h"

#include <QComboBox>

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"
#include "CParsedFileModel.h"

#include <QListWidget>
#ifdef ENABLE_DBMANAGER
#include "WSqlMultiTable.h"
#include "CSqlMultiTableDelegate.h"
#include "CHorizontalProxyModel.h"
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

    CParsedFileModel *fileModel = new CParsedFileModel(*m_exportCSV, ui->TreeList_files);
    ui->TreeList_files->setModel(fileModel);

    // If there are loaded Esquema documents, associate the first one with the export CSV
    if (!esquemadocs->empty() && m_exportCSV->getAsocEsquema() == nullptr) {
        m_exportCSV->setAsocEsquema(esquemadocs->front()->getEsquema());
    }
    
    ui->spreadSheet_formatTable->setModel(m_exportCSV->getCsvTableModel());
    // m_exportCSV->getDbTableModel()->updateFields(CDbConnection::getConnection().getModel("Utility Bills")); new BOOKMARK


    ui->tableWidget_fileValues->insertColumn(0);
    ui->tableWidget_fileValues->insertColumn(1);

    // Set the header items
    QTableWidgetItem *HField = new QTableWidgetItem("Fields");
    ui->tableWidget_fileValues->setHorizontalHeaderItem(0, HField);

    QTableWidgetItem *HValues = new QTableWidgetItem("Values");
    ui->tableWidget_fileValues->setHorizontalHeaderItem(1, HValues);

    // Connect TreeList_files with tableWidget_fileValues so the last reflects contents of file selection form the first.
    connect(ui->TreeList_files, &WLoadedFilesTreeView::indexChanged, this, [=](const QModelIndex& current, const QModelIndex& previous) {
        Q_UNUSED(previous);

        // Clear table
        ui->tableWidget_fileValues->clearContents();
        ui->tableWidget_fileValues->setRowCount(0);

        // Get data from the model
        QVector<QString> keys = fileModel->data(current, Qt::UserRole).toStringList().toVector();
        QVector<QString> values = fileModel->data(current, Qt::UserRole + 1).toStringList().toVector();

        // Check that sizes match
        int keySize = keys.size();
        int valueSize = values.size();
        int rowCount = qMin(keySize, valueSize); // Ensure we don't go out of bounds if sizes differ

        // Insert rows and populate the table
        for (int i = 0; i < rowCount; ++i) {
            // Insert a new row
            ui->tableWidget_fileValues->insertRow(i);

            // Create new QTableWidgetItem for keys and values
            QTableWidgetItem *kItem = new QTableWidgetItem(keys.at(i));
            ui->tableWidget_fileValues->setItem(i, 0, kItem); // Add key to column 0

            QTableWidgetItem *vItem = new QTableWidgetItem(values.at(i));
            ui->tableWidget_fileValues->setItem(i, 1, vItem); // Add value to column 1
        }
    });


#ifdef ENABLE_DBMANAGER
    // Create CRecTable for sql insertions. Used if parseDB is selected
    WSqlMultiTable *dbTable = new WSqlMultiTable(ui->stackedWidget_parseOpt);
    m_dbProxyModel = new CHorizontalProxyModel(this);
    m_dbProxyModel->setSourceModel(m_exportCSV->getDbTableModel());
    // Disable custom widgets in order to only get line edits on every cell.
    CSqlMultiTableDelegate* dbTableDelegate = qobject_cast<CSqlMultiTableDelegate*>(dbTable->itemDelegate());
    dbTableDelegate->disableCustomWidgets(true);

    // Set the recModel generated and sotred by the exportCSV as the table model.
    dbTable->setModel(m_dbProxyModel);

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
