#include "DDisplayParsedInfo.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include "document/CMDoc.h"
#include "qsqlerror.h"
#ifdef ENABLE_DBMANAGER
#include "dbManager/CSqlMultiTableModel.h"
#include <QCryptographicHash>
#endif
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
    CSqlMultiTableModel *pSqlModel = dynamic_cast<CSqlMultiTableModel*>(m_combinedModel);
    if (pSqlModel) {
        connect(exportCSVAction, &QAction::triggered, this, &DDisplayParsedInfo::commitTable);
    } else {
        connect(exportCSVAction, &QAction::triggered, this, &DDisplayParsedInfo::exportToCSV);
    }
    connect(exportDBAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, "Not Implemented", "Not implemented yet"); // BOOKMARK - Needs implementation
    });

    // Set up the layout
    this->layout()->setMenuBar(menuBar);
    ui->spreadSheet->setModel(m_combinedModel);
}

DDisplayParsedInfo::~DDisplayParsedInfo() {
    delete ui;
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

#ifdef ENABLE_DBMANAGER


QString computePdfHash(const QByteArray &pdfData) {
    return QCryptographicHash::hash(pdfData, QCryptographicHash::Sha256).toHex();
}

QByteArray readPdfFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return QByteArray();
    }
    return file.readAll();
}



bool DDisplayParsedInfo::commitTable() {
    CSqlMultiTableModel *pSqlModel = dynamic_cast<CSqlMultiTableModel*>(m_combinedModel);
    QSqlDatabase db = QSqlDatabase::database("closca");
    if(pSqlModel) {
        // Start a transaction
        if (!db.transaction()) {
            qWarning() << "Failed to start database transaction:" << db.lastError().text();
            return false;
        }
    } else return false;

    for (int row = 0; row < pSqlModel->rowCount(); ++row) {
        // Collect metadata from the table model
        QString bill_num = pSqlModel->data(pSqlModel->index(row, 0)).toString();
        QString exp_date = pSqlModel->data(pSqlModel->index(row, 1)).toString();
        QString billing_periode_start = pSqlModel->data(pSqlModel->index(row, 2)).toString();
        QString billing_periode_end = pSqlModel->data(pSqlModel->index(row, 3)).toString();

        QString floor_num = pSqlModel->data(pSqlModel->index(row, 5)).toString();
        QString door_num = pSqlModel->data(pSqlModel->index(row, 6)).toString();
        QString utility_name = pSqlModel->data(pSqlModel->index(row, 7)).toString();
        QString company_cif = pSqlModel->data(pSqlModel->index(row, 4)).toString();
        double iva = pSqlModel->data(pSqlModel->index(row, 8)).toDouble();
        double bi = pSqlModel->data(pSqlModel->index(row, 9)).toDouble();
        double total = pSqlModel->data(pSqlModel->index(row, 10)).toDouble();

        // Fetch flat_id and utility_id based on floor_num, door_num, utility_name
        // Lambda to get flat_id based on floor_num and door_num
        auto getFlatID = [&](const QString& floor_num, const QString& door_num) -> int {
            QSqlQuery query(db);
            query.prepare("SELECT flat_id FROM flats WHERE floor_num = :floor_num AND door_num = :door_num");
            query.bindValue(":floor_num", floor_num);
            query.bindValue(":door_num", door_num);

            if (query.exec() && query.next()) {
                return query.value(0).toInt();  // Return flat_id
            } else {
                // Handle case where flat_id is not found, return a sentinel value, or insert a new row
                qWarning() << "Flat "<< QString("%1-%2").arg(floor_num).arg(door_num) << "  not found.";
                return -1;
            }
        };

        // Lambda to get utility_id based on utility_name
        auto getUtilityID = [&](const QString& utility_name) -> int {
            QSqlQuery query(db);
            query.prepare("SELECT utility_id FROM utility WHERE utility_name = :utility_name");
            query.bindValue(":utility_name", utility_name);

            if (query.exec() && query.next()) {
                return query.value(0).toInt();  // Return utility_id
            } else {
                // Handle case where utility_id is not found, return a sentinel value, or insert a new row
                qWarning() << "Utility not found";
                return -1;
            }
        };

        int flat_id = getFlatID(floor_num, door_num);
        int utility_id = getUtilityID(utility_name);

        // Insert into ru_bills
        QSqlQuery query(db);
        query.prepare("INSERT INTO ru_bills (bill_num, exp_date, flat_id, billing_periode_start, billing_periode_end, company_cif, utility_id, iva, bi, total) "
                      "VALUES (:bill_num, :exp_date, :flat_id, :billing_periode_start, :billing_periode_end, :company_cif, :utility_id, :iva, :bi, :total)");

        query.bindValue(":bill_num", bill_num);
        query.bindValue(":exp_date", exp_date);
        query.bindValue(":flat_id", flat_id);
        query.bindValue(":billing_periode_start", billing_periode_start);
        query.bindValue(":billing_periode_end", billing_periode_end);
        query.bindValue(":company_cif", company_cif);
        query.bindValue(":utility_id", utility_id);
        query.bindValue(":iva", iva);
        query.bindValue(":bi", bi);
        query.bindValue(":total", total);

        if (!query.exec()) {
            qWarning() << "Failed to insert into ru_bills:" << query.lastError().text();
            db.rollback();
            return false;
        }

        // Commit the transaction if all queries succeeded
        if (!db.commit()) {
            qWarning() << "Failed to commit database transaction:" << db.lastError().text();
            return false;
        }
    }

    return true;
}
#endif

void DDisplayParsedInfo::on_pushButton_save_clicked() {
#ifdef ENABLE_DBMANAGER
    CSqlMultiTableModel *pSqlModel = dynamic_cast<CSqlMultiTableModel*>(m_combinedModel);
    if (pSqlModel && commitTable()) {
        accept();
    }
#endif
    // if(exportToCSV()) { accept(); }
}
