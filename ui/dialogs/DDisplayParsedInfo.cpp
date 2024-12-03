/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "DDisplayParsedInfo.h"
#include "ui/dialogs/ui_DDisplayParsedInfo.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "document/CMDoc.h"
#include "CParsedPdfModel.h"

#ifdef ENABLE_DBMANAGER
#include <QSqlError>
#include "sql_queries.h"
#include "utils/UText.h"
#include "CDbConnection.h"
#endif

DDisplayParsedInfo::DDisplayParsedInfo(CParsedPdfModel *combinedModel, bool isCSVParser, QWidget *parent)
    : QDialog(parent), ui(new Ui::DDisplayParsedInfo), m_combinedModel{combinedModel}, m_isCSVParser{isCSVParser}
{
    ui->setupUi(this);
    // Create a menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = menuBar->addMenu(tr("File"));

    QAction *exportCSVAction = new QAction(tr("Export to csv"), this);
    fileMenu->addAction(exportCSVAction);
    QAction *exportDBAction = new QAction(tr("Export to Database"), this);
    fileMenu->addAction(exportDBAction);

    connect(exportCSVAction, &QAction::triggered, this, &DDisplayParsedInfo::commitTable);
    connect(exportDBAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, "Not Implemented", "Not implemented yet");
    });

    this->layout()->setMenuBar(menuBar);
    ui->spreadSheet->setModel(m_combinedModel);
}

DDisplayParsedInfo::~DDisplayParsedInfo() {
    delete ui;
}

bool DDisplayParsedInfo::exportToCSV() {
    QString saveCSVFileName = QFileDialog::getSaveFileName(nullptr, "Save File", QDir::homePath(), "Comma separated values (*.csv)");
    if (saveCSVFileName.isEmpty()) { return false; }
    CMDoc& cmdoc = CMDoc::getMDoc();
    cmdoc.getExportPathDoc().modelToFile(saveCSVFileName, m_combinedModel);
    return true;
}

#ifdef ENABLE_DBMANAGER

QByteArray readPdfFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return QByteArray();
    }
    return file.readAll();
}

bool DDisplayParsedInfo::commitTable() {
    QSqlDatabase db = QSqlDatabase::database("closca");
    auto& model = *m_combinedModel;

    if (!db.transaction()) {
        qWarning() << "Failed to start database transaction:" << db.lastError().text();
        return false;
    }

    qDebug() << "Start transaction";

    QStringList existingBills;
    QStringList newEntries;

    for (int row = 0; row < model.rowCount(); ++row) {
        QString pdfHash = model.getFileMetadata(row)->getPdfHash();

        // Check if the PDF hash already exists
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT bill_num FROM ru_bills WHERE pdf_hash = :pdf_hash");
        checkQuery.bindValue(":pdf_hash", pdfHash);

        if (checkQuery.exec() && checkQuery.next()) {
            existingBills.append(checkQuery.value(0).toString());
        } else {
            newEntries.append(pdfHash);
        }
    }

    if (!existingBills.isEmpty()) {
        QMessageBox::StandardButton reply;
        QString msg = QString("The following bills already exist:\n%1\n\nDo you want to update them?")
                          .arg(existingBills.join("\n"));
        reply = QMessageBox::question(this, "Existing Bills", msg,
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            // Only insert new entries
            insertEntries(db, model, newEntries);
            db.commit();
            return true;
        }
    }

    // Proceed with upsert for all entries
    for (int row = 0; row < model.rowCount(); ++row) {
        QString pdfHash = model.getFileMetadata(row)->getPdfHash();
        newEntries.append(pdfHash);
    }

    // Upsert all entries (including existing)
    insertEntries(db, model, newEntries);

    if (!db.commit()) {
        qWarning() << "Failed to commit database transaction:" << db.lastError().text();
        return false;
    }

    qDebug() << "Transaction committed successfully";
    return true;
}

void DDisplayParsedInfo::insertEntries(QSqlDatabase &db, CParsedPdfModel &model, const QStringList &pdfHashes) {
    for (const QString &pdfHash : pdfHashes) {
        int row = -1;
        for (int r = 0; r < model.rowCount(); ++r) {
            if (model.getFileMetadata(r)->getPdfHash() == pdfHash) {
                row = r;
                break;
            }
        }

        if (row == -1) {
            continue; // Skip if not found
        }

        // Collect metadata for the entry
        QString bill_num                = model.data(model.index(row, 0)).toString();
        QString exp_date                = model.data(model.index(row, 1)).toString();
        QString billing_periode_start   = model.data(model.index(row, 2)).toString();
        QString billing_periode_end     = model.data(model.index(row, 3)).toString();
        QString floor_num               = model.data(model.index(row, 5)).toString();
        QString door_num                = model.data(model.index(row, 6)).toString();
        QString utility_name            = model.data(model.index(row, 7)).toString();
        QString company_cif             = model.data(model.index(row, 4)).toString();
        double iva                      = UText::trimCurrency(model.data(model.index(row, 8)).toString());
        double bi                       = UText::trimCurrency(model.data(model.index(row, 9)).toString());
        double total                    = UText::trimCurrency(model.data(model.index(row, 10)).toString());

        QByteArray pdfData  = readPdfFile(model.getFileMetadata(row)->filePath());
        int flat_id         = getFlatID(db, floor_num, door_num);
        int utility_id      = getUtilityID(db, utility_name);

        // Prepare the insert query
        QSqlQuery insertQuery(db);
        insertQuery.prepare(SqlQueries::UpsertRuBills);
        insertQuery.bindValue(":pdf_hash",  pdfHash);
        insertQuery.bindValue(":bill_num",  bill_num);
        insertQuery.bindValue(":exp_date",  exp_date);
        insertQuery.bindValue(":flat_id" ,  flat_id);
        insertQuery.bindValue(":pdf_bill",  pdfData);
        insertQuery.bindValue(":billing_periode_start", billing_periode_start);
        insertQuery.bindValue(":billing_periode_end",   billing_periode_end);
        insertQuery.bindValue(":company_cif", company_cif);
        insertQuery.bindValue(":utility_id" , utility_id);
        insertQuery.bindValue(":iva", iva);
        insertQuery.bindValue(":bi" , bi);
        insertQuery.bindValue(":total", total);

        // Execute the insert query
        if (!insertQuery.exec()) {
            qWarning() << "Failed to insert new entry into ru_bills:" << insertQuery.lastError().text();
            db.rollback();
            return;
        }
    }
}

int DDisplayParsedInfo::getFlatID(QSqlDatabase &db, const QString &floor_num, const QString &door_num) {
    QSqlQuery query(db);
    query.prepare("SELECT flat_id FROM flats WHERE floor_num = :floor_num AND door_num = :door_num");
    query.bindValue(":floor_num", floor_num);
    query.bindValue(":door_num", door_num);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Flat " << QString("%1-%2").arg(floor_num, door_num) << " not found.";
        return -1;
    }
}

int DDisplayParsedInfo::getUtilityID(QSqlDatabase &db, const QString &utility_name) {
    QSqlQuery query(db);
    query.prepare("SELECT utility_id FROM utility WHERE utility_name = :utility_name");
    query.bindValue(":utility_name", utility_name);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Utility not found";
        return -1;
    }
}

#endif

void DDisplayParsedInfo::on_pushButton_save_clicked() {
#ifdef ENABLE_DBMANAGER
    if(m_isCSVParser) {
        if(exportToCSV()) accept();
    } else if (commitTable()) {
        CDbConnection &conn = CDbConnection::getConnection("closca");
        conn.refreshModel("Utility Bills");
        accept();
    }
#endif
}
