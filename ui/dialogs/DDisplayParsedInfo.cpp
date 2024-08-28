#include "DDisplayParsedInfo.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include "document/CMDoc.h"
#include "ui/dialogs/ui_DDisplayParsedInfo.h"

DDisplayParsedInfo::DDisplayParsedInfo(QAbstractItemModel *combinedModel,  QWidget *parent)
    : QDialog(parent), ui(new Ui::DDisplayParsedInfo), m_combinedModel{combinedModel}
{
    ui->setupUi(this);

    // Create a menu bar
    QMenuBar *menuBar = new QMenuBar(this);

    // Create a File menu
    QMenu *fileMenu = menuBar->addMenu(tr("File"));

    // Add actions to the File menu
    QAction *exportCSVAction = new QAction(tr("Export to csv"), this);
    fileMenu->addAction(exportCSVAction);

    QAction *exportDBAction = new QAction(tr("Export to Database"), this);
    fileMenu->addAction(exportDBAction);

    // Connect actions to slots
    connect(exportCSVAction, &QAction::triggered, this, &DDisplayParsedInfo::exportToCSV);
    connect(exportDBAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, "Not Implemented", "Not implemented yet"); // BOOKMARK - Needs implementation
    });

    // Set up the layout
    this->layout()->setMenuBar(menuBar);
    ui->spreadSheet->setModel(combinedModel);
}

DDisplayParsedInfo::~DDisplayParsedInfo() {
    delete ui;
    delete m_combinedModel;
}

bool DDisplayParsedInfo::exportToCSV() {
    // Create .csv File from the structure
    // FILE BROWSE DIALOG FOR NAMING EXPORTED FILE
    // Open a file dialog for saving exported csv file
    QString saveCSVFileName = QFileDialog::getSaveFileName(nullptr, "Save File", QDir::homePath(), "Coma separated values (*.csv)");
    qDebug() << saveCSVFileName;
    if (saveCSVFileName.isEmpty()) { return false; } // Return if canceled
    ///////////////////////////////////////////////
    CMDoc& cmdoc = CMDoc::getMDoc();
    cmdoc.getExportPathDoc().modelToFile(saveCSVFileName, m_combinedModel);
    qDebug() << "exported";
    return true;
}
