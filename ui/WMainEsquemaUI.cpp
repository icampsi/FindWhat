#include "WMainEsquemaUI.h"
#include "ui_WMainEsquemaUI.h"
#include "PEsquemaPage.h"
#include "WFormExpToolBoxPage.h"
#include "document/CMDoc.h"
#include <QToolBox>

WMainEsquemaUI::WMainEsquemaUI(QWidget *parent)
    : QWidget(parent), ui(new Ui::WMainEsquemaUI)
{
    ui->setupUi(this);
    connect(ui->toolBar, &WToolBarEsquema::previewOptionChanged, this, &WMainEsquemaUI::esquemaOptionChanged);
    EsquemaPage *newPage = new EsquemaPage(this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->stackedWidget_esquemaUI->setCurrentIndex(0);
    ui->toolBox_formatEsquema->removeItem(0); // Needed since the .ui file interface doesent allow for empty pages in the toolbox widget
}

WMainEsquemaUI::~WMainEsquemaUI() { delete ui; }

void WMainEsquemaUI::newEsquema(CEsquemaDoc* esquemaDoc) {
    CEsquema* esquema = esquemaDoc->getEsquema();
    EsquemaPage *newPage = new EsquemaPage(esquemaDoc, this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);// tabWidget_esquemaTab->addTab(newPage, esquema->m_nameEsquema);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->loadedEsquemes->newEsquema(newPage, esquema);
}

void WMainEsquemaUI::esquemaOptionChanged(WToolBarEsquema::EsquemaOption option) {
    ui->stackedWidget_esquemaUI->setCurrentIndex(static_cast<int>(option));
}

void WMainEsquemaUI::changeCurrentPage(EsquemaPage* page) {
    ui->stackedWidget_esquemaPage->setCurrentWidget(page);
}

void WMainEsquemaUI::on_pushButton_addEsquema_clicked() {
    // Disable the button until actions are finished so there are no conflicts witht he document types we acces on the toolboxpage constructor
    ui->pushButton_addEsquema->setEnabled(false);
    // Create a new CExport and a ToolBox page instances and binds them together
    WFormExpToolBoxPage *newToolBoxPage = new WFormExpToolBoxPage(ui->toolBox_formatEsquema);
    ui->toolBox_formatEsquema->addItem(newToolBoxPage, "New Page"); // Still need a way to rename pages so it has more sense. Maybe with the name of the used Esquema on this page
    ui->toolBox_formatEsquema->setCurrentWidget(newToolBoxPage);
    ui->pushButton_addEsquema->setEnabled(true);
}

void WMainEsquemaUI::on_DeleteEsquema_clicked() {
    int index = ui->toolBox_formatEsquema->currentIndex();
    // Remove the asociated document
    CMDoc::getMDoc().getExportPathDoc().deleteExportCSV(index);
    // Remove the page from the toolBox
    ui->toolBox_formatEsquema->removeItem(index);
}

void WMainEsquemaUI::on_pushButton_clicked() {
    ui->pushButton->setEnabled(false);
    std::vector<std::vector<CData*>> xsvStructure;
    // Get all the loaded exportCSV as a vector
    std::vector<CExportCSV*> exportCSVs = CMDoc::getMDoc().getExportPathDoc().getExportCSVs();
    std::vector<std::vector<CData*>> newData;
    for (CExportCSV* it : exportCSVs) {
        // Pass the string taken from the textedit in the WFormExpToolBoxPage to the selected esquema
        it->getAsocEsquemaDoc()->getEsquema()->setCsvFormatFormula(it->getCSVFormat(), '\"',',');
        // Build the structure
        newData = it->buildXSVStructure();
        // Append it to the global file structure
        xsvStructure.insert(xsvStructure.end(), newData.begin(), newData.end());
    }
    // Create .csv File from the structure
    CMDoc::getMDoc().getExportPathDoc().xsvm_structureToFile("prova.csv", xsvStructure, ',');
    ui->pushButton->setEnabled(true);
    QMessageBox::information(this,"Succes!", ".csv file created succesfully");
}


void WMainEsquemaUI::handleFilePathChanged(const QString &filePath) {
    dynamic_cast<EsquemaPage*>(ui->stackedWidget_esquemaPage->currentWidget())->updateFunctionProcess();
}
