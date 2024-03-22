/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PMainEsquemaUI.h"
#include "qtpreprocessorsupport.h"
#include "MainWindow.h"
#include "ui_PMainEsquemaUI.h"

#include <QToolBox>

#include "PEsquemaPage.h"
#include "PFormExpToolBoxPage.h"
#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"
#include "utils/generalfunctions.h"

PMainEsquemaUI::PMainEsquemaUI(QWidget *parent)
    : QWidget(parent), ui(new Ui::PMainEsquemaUI), m_emptyPage(this)
{
    ui->setupUi(this);

    // CONNECTION
    connect(ui->toolBar, &WToolBarEsquema::previewOptionChanged, this, &PMainEsquemaUI::esquemaOptionChanged);

    // Set esquemaPage to empty and remove .ui generated toolBox_formatEsquema page
    setCurrentPageToEmptyPage();
    ui->toolBox_formatEsquema->removeItem(0); // Needed since the .ui file interface doesent allow for empty pages in the toolbox widget

    // Disable "export CSV" and "delete esquema" buttons if no toolbox pages are loaded
    if(ui->toolBox_formatEsquema->count() == 0) {
        ui->pushButton_ExportCSV->setEnabled(false);
        ui->DeleteEsquema->setEnabled(false);
    }
}

PMainEsquemaUI::~PMainEsquemaUI() { delete ui; }

void PMainEsquemaUI::newEsquema(CEsquemaDoc* esquemaDoc) {
    CEsquema* esquema = esquemaDoc->getEsquema();
    PEsquemaPage *newPage = new PEsquemaPage(esquemaDoc, this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->loadedEsquemes->newEsquema(newPage, esquema);
}

void PMainEsquemaUI::on_pushButton_addEsquema_clicked() {
    // Disable the button until actions are finished so there are no conflicts witht he document types we acces on the toolboxpage constructor
    ui->pushButton_addEsquema->setEnabled(false);
    // Create a new CExport and a ToolBox page instances and binds them together
    PFormExpToolBoxPage *newToolBoxPage = new PFormExpToolBoxPage(ui->toolBox_formatEsquema);
    ui->toolBox_formatEsquema->addItem(newToolBoxPage, "New Page"); // Still need a way to rename pages so it has more sense. Maybe with the name of the used Esquema on this page
    ui->toolBox_formatEsquema->setCurrentWidget(newToolBoxPage);

    // Enable export esquema button if any page is loaded
    switch (ui->toolBox_formatEsquema->count()) {
    case 0:
        ui->pushButton_ExportCSV->setEnabled(false);
        break;
    case 1:
        ui->pushButton_ExportCSV->setEnabled(true);
        ui->DeleteEsquema->setEnabled(true);
        break;
    }

    ui->pushButton_addEsquema->setEnabled(true);
}

void PMainEsquemaUI::on_DeleteEsquema_clicked() {
    int index = ui->toolBox_formatEsquema->currentIndex();
    // Remove the asociated document
    CMDoc::getMDoc().getExportPathDoc().deleteExportCSV(index);
    // Remove the page from the toolBox
    ui->toolBox_formatEsquema->removeItem(index);

    // Select last page
    int count = ui->toolBox_formatEsquema->count();
    if (count > 1) ui->toolBox_formatEsquema->setCurrentIndex(count - 1);

    // Disable export esquema button if no pages are loaded
    switch (count) {
    case 0:
        ui->pushButton_ExportCSV->setEnabled(false);
        ui->DeleteEsquema->setEnabled(false);
        break;
    case 1:
        ui->pushButton_ExportCSV->setEnabled(true);
        break;
    }
}

void PMainEsquemaUI::on_pushButton_ExportCSV_clicked() {
    if (CMDoc::getMDoc().getLoadedEsquemaDocs()->size() == 0) {
        QMessageBox::information(this, "Empty esquema", "You need to define at least one esquema to extract data from PDF files");
        return;
    }
    ui->pushButton_ExportCSV->setEnabled(false);
    std::vector<std::vector<QString>> xsvStructure;
    // Get all the loaded exportCSV as a vector
    std::vector<CExportCSV*> exportCSVs = CMDoc::getMDoc().getExportPathDoc().getExportCSVs();

    // Checks the ammount of work that will be needed to set up the progress bar dialog (and to check if actually anything is needed)
    size_t fileCount = 0;
    for (CExportCSV* it : exportCSVs) {
        fileCount += it->getFilePaths().size();
    }
    // Creates a progressBar dialog and sets its progress bar range to match fileCount
    if(fileCount > 0) {
        ProgBarExport_dlg *progressDlg = new ProgBarExport_dlg(fileCount, this);
        progressDlg->show();
        // Process pending events to update the UI
        QCoreApplication::processEvents();

        std::vector<std::vector<QString>> newData;
        for (CExportCSV* it : exportCSVs) {
            // Pass the string taken from the textedit in the WFormExpToolBoxPage to the selected esquema
            it->getAsocEsquemaDoc()->getEsquema()->constructCsvFormatFormulaStructure(it->getCSVFormat(), '\"',',');
            // Build the structure
            it->buildXSVStructure(&newData, progressDlg);
            // Append it to the global xsvm structure
            xsvStructure.insert(xsvStructure.end(), newData.begin(), newData.end());
        }
        // Create .csv File from the structure
        CMDoc::getMDoc().getExportPathDoc().xsvm_stringStructureToFile("extractedText.csv", xsvStructure, ',');
        // Delete progress dialog for closing
        delete progressDlg;
        progressDlg = nullptr;
    }

    ui->pushButton_ExportCSV->setEnabled(true);
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
    ui->stackedWidget_esquemaUI->setCurrentIndex(0);
    ui->loadedEsquemes->deleteEsquema(index);
    CMDoc::getMDoc().deleteEsquema(index);
    // Perform a check in main window for disableing Export Esquema menu action if needed
    qobject_cast<MainWindow*>(getLastParent(this))->checkExortEsquemaActionEnable();
    qDebug() << ui->stackedWidget_esquemaPage->count();
}

void PMainEsquemaUI::setCurrentPageToEmptyPage() {
    ui->stackedWidget_esquemaPage->addWidget(&m_emptyPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(&m_emptyPage);
    ui->stackedWidget_esquemaUI->setCurrentIndex(0);
}
