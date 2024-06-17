/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PMainEsquemaUI.h"

#include "MainWindow.h"
#include "ui/dialogs/DDisplayParsedInfo.h"
#include "ui/dialogs/ProgBarExport_dlg.h"
#include "ui_PMainEsquemaUI.h"

#include <QToolBox>
#include <QtPreprocessorSupport>
#include <QFileDialog>

#include "PEsquemaPage.h"
#include "PFormExpToolBoxPage.h"
#include "WFuncList.h"

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"
#include "document/CExportPathDoc.h"

#include "utils/generalfunctions.h"

PMainEsquemaUI::PMainEsquemaUI(QWidget *parent)
    : QWidget(parent), ui(new Ui::PMainEsquemaUI), m_emptyPage(this)
{
    ui->setupUi(this);

    // CONNECTION
    connect(ui->toolBar, &WToolBarEsquema::previewOptionChanged, this, &PMainEsquemaUI::esquemaOptionChanged);

    // Set esquemaPage to empty and remove .ui generated toolBox_formatEsquema page
    ui->stackedWidget_esquemaPage->addWidget(&m_emptyPage);
    setCurrentPageToEmptyPage();
    ui->stackedWidget_esquemaUI->setCurrentIndex(0);

    ui->toolBox_formatEsquema->removeItem(0); // Needed since the .ui file interface doesent allow for empty pages in the toolbox widget

    // Disable "export CSV" and "delete esquema" buttons if no toolbox pages are loaded
    if(ui->toolBox_formatEsquema->count() == 0) {
        ui->pushButton_parse->setEnabled(false);
        ui->DeletePage->setEnabled(false);
    }
}

PMainEsquemaUI::~PMainEsquemaUI() { delete ui; }

void PMainEsquemaUI::newEsquema(CEsquemaDoc* esquemaDoc) {
    CEsquema* esquema = esquemaDoc->getEsquema();
    PEsquemaPage *newPage = new PEsquemaPage(esquemaDoc, this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->esqList->newEsquema(newPage, esquema);
}

void PMainEsquemaUI::addExportCSV(CExportCSV *exportCSV) {

    // Create a new CExport and a ToolBox page instances and binds them together
    PFormExpToolBoxPage *newToolBoxPage = new PFormExpToolBoxPage(ui->toolBox_formatEsquema, exportCSV);
    ui->toolBox_formatEsquema->addItem(newToolBoxPage, "New Page"); // BOOKMARK - Still need a way to rename pages so it has more sense. Maybe with the name of the used Esquema on this page
    ui->toolBox_formatEsquema->setCurrentWidget(newToolBoxPage);

    // Enable export esquema button if any page is loaded
    switch (ui->toolBox_formatEsquema->count()) {
    case 0:
        ui->pushButton_parse->setEnabled(false);
        break;
    case 1:
        ui->pushButton_parse->setEnabled(true);
        ui->DeletePage->setEnabled(true);
        break;
    }
}

void PMainEsquemaUI::on_pushButton_addPage_clicked() {
    // Disable the button until actions are finished so there are
    // no conflicts with he document types we acces on the toolboxpage constructor
    ui->pushButton_addPage->setEnabled(false);

    addExportCSV(nullptr);

    ui->pushButton_addPage->setEnabled(true);
}

void PMainEsquemaUI::deletePage(int index) {
    // Remove the associated document
    CMDoc::getMDoc().getExportPathDoc().deleteExportCSV(index);

    // Remove the page from the toolBox
    ui->toolBox_formatEsquema->removeItem(index);

    // Select last page
    int count = ui->toolBox_formatEsquema->count();
    if (count > 1) ui->toolBox_formatEsquema->setCurrentIndex(count - 1);

    // Disable export esquema button if no pages are loaded
    switch (count) {
    case 0:
        ui->pushButton_parse->setEnabled(false);
        ui->DeletePage->setEnabled(false);
        break;
    case 1:
        ui->pushButton_parse->setEnabled(true);
        break;
    }
}

void PMainEsquemaUI::clearPages() {
    int pages = ui->toolBox_formatEsquema->count();
    for(int i{pages - 1}; i >= 0; i--) {
        deletePage(i);
    }
}

void PMainEsquemaUI::on_pushButton_parse_clicked() {
    CMDoc& cmdoc = CMDoc::getMDoc();
    if (cmdoc.getLoadedEsquemaDocs()->size() == 0) {
        QMessageBox::information(this, "Empty esquema list", "You need to define at least one esquema to extract data from PDF files");
        return;
    }

    ui->pushButton_parse->setEnabled(false); // Disable button to avoid conflicts while parsing

    std::vector<std::vector<QString>> xsvStructure; // Data Structure that will hold the whole parsed info

    CExportPathDoc& exportPathDoc = CMDoc::getMDoc().getExportPathDoc();
    // Get all the loaded exportCSV as a vector
    const std::vector<CExportCSV*>& exportCSVs = exportPathDoc.getExportCSVs();

    // Checks the ammount of work that will be needed (if any) to set up the progress bar dialog
    const size_t fileCount = exportPathDoc.getFileCount();

    // Creates progressBar dialog
    if(fileCount > 0) {
        ProgBarExport_dlg *progressDlg = new ProgBarExport_dlg(fileCount, this);
        progressDlg->show();
        QCoreApplication::processEvents(); // Needed to display progress bar

        size_t maxColumns{ 0 };
        for (CExportCSV* it : exportCSVs) {
            QStandardItemModel *model = it->getTableModel();
            size_t columnCount = model->columnCount();
            if(columnCount > maxColumns) maxColumns = columnCount;
        }
        std::vector<std::vector<QString>> newData;
        for (CExportCSV* it : exportCSVs) {
            newData.clear();
            // DOING THE ACTUAL WORK: Build the structure
            it->buildStructure(&newData, progressDlg, maxColumns);
            // Append it to the global xsvm structure
            xsvStructure.insert(xsvStructure.end(), newData.begin(), newData.end());
        }
        // Display Table DIalog
        DDisplayParsedInfo *piDialog = new DDisplayParsedInfo(xsvStructure, this);
        piDialog->exec();

        // Delete progress dialog for closing
        delete progressDlg;
        progressDlg = nullptr;
    }
    ui->pushButton_parse->setEnabled(true);
}

void PMainEsquemaUI::handleFilePathChanged(const QString &filePath) {
    Q_UNUSED(filePath);

    QWidget *currentWidget   = ui->stackedWidget_esquemaPage->currentWidget();
    PEsquemaPage *esquemaPage = qobject_cast<PEsquemaPage*>(currentWidget);

    if (esquemaPage) {
        esquemaPage->updateFunctionProcess();
    } else {
        qDebug() << "Current widget is not an instance of EsquemaPage";
    }
}

void PMainEsquemaUI::handleDeleteEsquema(const size_t index) {
    // Delete esquema from ui and document
    ui->esqList->delEsq(index);
    CMDoc::getMDoc().deleteEsquema(index);
    // Perform a check in main window for disableing Export Esquema menu action if needed
    qobject_cast<MainWindow*>(getLastParent(this))->checkExortEsquemaActionEnable();
}
