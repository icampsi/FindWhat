#include "WMainEsquemaUI.h"
#include "qtpreprocessorsupport.h"
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

    // Checks the ammount of work that will be needed to set up the progress bar dialog (and to check if actually anything is needed)
    int fileCount = 0;
    for (CExportCSV* it : exportCSVs) {
        fileCount += it->getFilePaths().size();
    }
    // Creates a progressBar dialog and sets its progress bar range to match fileCount
    if(fileCount > 0) {
        exportCSVProgressBar_dlg *progressDlg = new exportCSVProgressBar_dlg(fileCount, this);
        progressDlg->show();
        // Process pending events to update the UI
        QCoreApplication::processEvents();

        std::vector<std::vector<CData*>> newData;
        for (CExportCSV* it : exportCSVs) {
            // Pass the string taken from the textedit in the WFormExpToolBoxPage to the selected esquema
            it->getAsocEsquemaDoc()->getEsquema()->setCsvFormatFormula(it->getCSVFormat(), '\"',',');
            // Build the structure
            newData = it->buildXSVStructure(progressDlg);
            // Append it to the global xsvm structure
            xsvStructure.insert(xsvStructure.end(), newData.begin(), newData.end());
        }
        // Create .csv File from the structure
        CMDoc::getMDoc().getExportPathDoc().xsvm_structureToFile("extractedText.csv", xsvStructure, ',');
        // Delete progress dialog for closing
        delete progressDlg;
        progressDlg = nullptr;
    }

    ui->pushButton->setEnabled(true);
}
void WMainEsquemaUI::handleFilePathChanged(const QString &filePath) {
    Q_UNUSED(filePath);

    QWidget *currentWidget   = ui->stackedWidget_esquemaPage->currentWidget();
    EsquemaPage *esquemaPage = qobject_cast<EsquemaPage*>(currentWidget);

    if (esquemaPage) {
        esquemaPage->updateFunctionProcess();
    } else {
        qDebug() << "Current widget is not an instance of EsquemaPage";
    }
}
