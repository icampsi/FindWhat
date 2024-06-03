#include "DDisplayParsedInfo.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include "document/CMDoc.h"

DDisplayParsedInfo::DDisplayParsedInfo(std::vector<std::vector<QString>> &xsvStruct, QWidget *parent)
    : QDialog(parent), ui(new Ui::DDisplayParsedInfo), m_xsvStruct{xsvStruct}
{
    // Create a menu bar
    QMenuBar *menuBar = new QMenuBar(this);

    // Create a File menu
    QMenu *fileMenu = menuBar->addMenu(tr("File"));

    // Add actions to the File menu
    QAction *exportCSVAction = new QAction(tr("Export to csv"), this);
    fileMenu->addAction(exportCSVAction);

    QAction *exportDBAction = new QAction(tr("Export to Database"), this);
    fileMenu->addAction(exportDBAction);

    ui->setupUi(this);
    populateTable(xsvStruct);

    // Connect actions to slots
    connect(exportCSVAction, &QAction::triggered, this, &DDisplayParsedInfo::exportToCSV);
    connect(exportDBAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, "Not Implemented", "Not implemented yet"); // BOOKMARK - Needs implementation
    });

    // Set up the layout
    this->layout()->setMenuBar(menuBar);
}

DDisplayParsedInfo::~DDisplayParsedInfo() { delete ui; }


void DDisplayParsedInfo::populateTable(std::vector<std::vector<QString>>& xsvStruct) {
    int numRows = static_cast<int>(xsvStruct.size());
    if (numRows == 0) return;

    int numCols = static_cast<int>(xsvStruct[0].size());
    ui->tableWidget->setRowCount(numRows);
    ui->tableWidget->setColumnCount(numCols);

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(xsvStruct[row][col]);
            ui->tableWidget->setItem(row, col, item);
        }
    }
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
    cmdoc.getExportPathDoc().xsvm_stringStructureToFile(saveCSVFileName, m_xsvStruct, ',');
    qDebug() << "exported";
    return true;
}
