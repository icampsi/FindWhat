#include "PMainEsquemaUI.h"
#include "qtpreprocessorsupport.h"
#include "ui_PMainEsquemaUI.h"
#include "PEsquemaPage.h"
#include "PFormExpToolBoxPage.h"
#include "document/CMDoc.h"
#include <QToolBox>

PMainEsquemaUI::PMainEsquemaUI(QWidget *parent)
    : QWidget(parent), ui(new Ui::PMainEsquemaUI)
{
    ui->setupUi(this);
    connect(ui->toolBar, &WToolBarEsquema::previewOptionChanged, this, &PMainEsquemaUI::esquemaOptionChanged);
    PEsquemaPage *newPage = new PEsquemaPage(this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->stackedWidget_esquemaUI->setCurrentIndex(0);
    ui->toolBox_formatEsquema->removeItem(0); // Needed since the .ui file interface doesent allow for empty pages in the toolbox widget
}

PMainEsquemaUI::~PMainEsquemaUI() { delete ui; }

void PMainEsquemaUI::newEsquema(CEsquemaDoc* esquemaDoc) {
    CEsquema* esquema = esquemaDoc->getEsquema();
    PEsquemaPage *newPage = new PEsquemaPage(esquemaDoc, this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);// tabWidget_esquemaTab->addTab(newPage, esquema->m_nameEsquema);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);
    ui->loadedEsquemes->newEsquema(newPage, esquema);
}

void PMainEsquemaUI::esquemaOptionChanged(WToolBarEsquema::EsquemaOption option) {
    ui->stackedWidget_esquemaUI->setCurrentIndex(static_cast<int>(option));
}

void PMainEsquemaUI::changeCurrentPage(PEsquemaPage* page) {
    ui->stackedWidget_esquemaPage->setCurrentWidget(page);
}

void PMainEsquemaUI::on_pushButton_addEsquema_clicked() {
    // Disable the button until actions are finished so there are no conflicts witht he document types we acces on the toolboxpage constructor
    ui->pushButton_addEsquema->setEnabled(false);
    // Create a new CExport and a ToolBox page instances and binds them together
    PFormExpToolBoxPage *newToolBoxPage = new PFormExpToolBoxPage(ui->toolBox_formatEsquema);
    ui->toolBox_formatEsquema->addItem(newToolBoxPage, "New Page"); // Still need a way to rename pages so it has more sense. Maybe with the name of the used Esquema on this page
    ui->toolBox_formatEsquema->setCurrentWidget(newToolBoxPage);
    ui->pushButton_addEsquema->setEnabled(true);
}

void PMainEsquemaUI::on_DeleteEsquema_clicked() {
    int index = ui->toolBox_formatEsquema->currentIndex();
    // Remove the asociated document
    CMDoc::getMDoc().getExportPathDoc().deleteExportCSV(index);
    // Remove the page from the toolBox
    ui->toolBox_formatEsquema->removeItem(index);
}

void PMainEsquemaUI::on_pushButton_clicked() {
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
        ExportCSVProgressBar_dlg *progressDlg = new ExportCSVProgressBar_dlg(fileCount, this);
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
