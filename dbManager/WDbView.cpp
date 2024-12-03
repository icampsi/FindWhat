/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WDbView.h"
#include "ui_WDbView.h"
#include "utils/USerialize.h"

#include "DMemberEdit.h"
#include "WSearcherHeader.h"
#include "CDbConnection.h"
#include "CSqlMultiTableModel.h"
#include "sql_queries.h"
#include "ui/WExtViews/WTreeView.h"

#include <QSortFilterProxyModel>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlIndex>
#include <QMenu>
#include <QKeyEvent>
#include <QMessageBox>

WDbView::WDbView(QWidget *parent)
    : QWidget(parent), ui(new Ui::WDbView) {
    ui->setupUi(this);

    // Create a QSortFilterProxyModel to enable filtering
    m_proxyModel = new QSortFilterProxyModel();
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    // Set the model for the table view
    ui->treeView->setModel(m_proxyModel);
    ui->treeView->setSortingEnabled(true);

    // Set custom header that includes a search box in each column
    m_head = new WSearcherHeader(Qt::Horizontal, ui->treeView);
    m_head->setModel(m_proxyModel);
    ui->treeView->setHeader(m_head);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Avoids edition on the table. We are supporting eddition on double click by opening a new dialog.

    // ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);  // Allows selection of entire rows
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // Single selection by default, multi-selection with Ctrl/Shift

    // Make connections to delete searchBoxes and create new ones everytime the header data changes
    connect(m_proxyModel, &QAbstractTableModel::modelReset, m_head, &WSearcherHeader::updateHeader);
    connect(m_head, &WSearcherHeader::filterChanged, this, &WDbView::filterChanged);

    changeTable("Members");
    // m_activeModel->insertRows(m_activeModel->rowCount(), 2);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeView, &WTreeView::customContextMenuRequested, this, &WDbView::showTreeContextMenu);
    connect(ui->treeView, &WTreeView::keyPressed, this, [this](QKeyEvent *event) {
        if(event->key() == Qt::Key_Delete) {
            // Show a confirmation message box
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Confirm Deletion",
                                          "Are you sure you want to delete the selected entries?",
                                          QMessageBox::Yes | QMessageBox::No);

            // If the user confirms, remove the selected rows
            if(reply == QMessageBox::Yes) {
                removeSelectedTreeViewRows();
            }
        }
    });

    connect(ui->pushButton_addEntry, &QPushButton::clicked, this, []() {
        // m_activeModel->
    } );

}

WDbView::~WDbView() {
    delete ui;
}

void WDbView::changeTable(const QString& tableName) {
    // Show every column since some could have gotten hiden before
    {
        int columnCount = ui->treeView->model()->columnCount();

        for (int i = 0; i < columnCount; ++i) {
            ui->treeView->showColumn(i);
        }
    }

    // Sets the new table, performs the select statement and hides/shows columns custimized
    m_activeModel = CDbConnection::getConnection().getModel(tableName);
    m_proxyModel->setSourceModel(m_activeModel);

    // Hide unnecessary columns
    using namespace SqlQueries;
    if(tableName == "Utility Bills") {
        ui->treeView->hideColumn(cIndex(GetRuBillsColumn::hash));
        ui->treeView->hideColumn(cIndex(GetRuBillsColumn::flat_id));
    } else if(tableName == "Occupancy") {
        ui->treeView->hideColumn(cIndex(OccupancyColumn::isMainHolder));
    }
}

void WDbView::on_comboBox_tables_currentIndexChanged(int index) {
    m_insertionOrder.clear();

    // Static array of table names
    static const char* tableNames[] = { "Members", "Flats", "Utility Bills", "Occupancy" };

    if (index < static_cast<int>(sizeof(tableNames) / sizeof(tableNames[0]))) {
        changeTable(tableNames[index]);
    } else {
        qDebug() << "WDbView::on_comboBox_tables_currentIndexChanged: \n\tTable index send from comboBox doesen't mach any listed table";
    }
}

bool WDbView::openEditDlg(const QModelIndex &index) {
    // Opens a dialog that displays the selected row info and allows updating
    if (!index.isValid() || !m_proxyModel) return false;

    // Retrieve proxy mapped index
    QModelIndex proxyIndex = m_proxyModel->mapToSource(index);

    // Create and show the dialog
    DMemberEdit *dialog = new DMemberEdit(m_activeModel, proxyIndex.row(), this);

    // If accepted requery to update the data
    if (dialog->exec() == QDialog::Accepted) {
        m_activeModel->requery();
    }

    delete dialog;
    return true;
}

void WDbView::on_treeView_doubleClicked(const QModelIndex &index) {
    if(!openEditDlg(index)) qWarning() << "Invalid index";
}

void WDbView::filterChanged(int column, const QString& text) {
    QRegularExpression regex(".*" + QRegularExpression::escape(text) + ".*", QRegularExpression::CaseInsensitiveOption);
    m_proxyModel->setFilterRegularExpression(std::move(regex));
    m_proxyModel->setFilterKeyColumn(column);
}

void WDbView::reconstructPDFFile(const QModelIndex &index) {
    QSqlDatabase db = QSqlDatabase::database("closca");

    // Extract necessary bill information to construct the file name
    using namespace SqlQueries;

    QModelIndex hashIndex = m_proxyModel->index(index.row(), cIndex(GetRuBillsColumn::hash));
    QString hash = m_proxyModel->data(hashIndex).toString();

    QModelIndex dateIndex = m_proxyModel->index(index.row(), cIndex(GetRuBillsColumn::exp_date));
    QString date = m_proxyModel->data(dateIndex).toString();

    QModelIndex companyIndex = m_proxyModel->index(index.row(), cIndex(GetRuBillsColumn::company_name));
    QString company = m_proxyModel->data(companyIndex).toString();

    QModelIndex flatIndex = m_proxyModel->index(index.row(), cIndex(GetRuBillsColumn::flat_id));
    QString flat = m_proxyModel->data(flatIndex).toString();

    QModelIndex utilityIndex = m_proxyModel->index(index.row(), cIndex(GetRuBillsColumn::utility_name));
    QString utility = m_proxyModel->data(utilityIndex).toString();

    // Generate the file name based on the row's data
    QString fileName = QString("%1_%2_%3 %4.pdf") // BOOKMARK - For now I haven't added a way to decide where to store the file.
                           .arg(date,
                                company,
                                flat,
                                utility);

    // Query to get the binary PDF data from the table
    QSqlQuery query(db);
    query.prepare("SELECT pdf_bill FROM ru_bills WHERE pdf_hash = :hash");
    query.bindValue(":hash", hash);

    if (!query.exec()) {
        qDebug() << "WDbView::reconstructPDFFile: \n\tFailed to execute query: " << query.lastError().text();
        return;
    }

    // Retrieve the PDF data as QByteArray
    if (query.next()) {
        QByteArray pdfData = query.value(0).toByteArray();

        // Reconstruct the file from the extracted data
        USerialize::fileFromBinary(fileName, pdfData);
    } else {
        qWarning() << "WDbView::reconstructPDFFile: \n\tNo data found for the given file ID.";
    }
}

bool WDbView::removeSelectedTreeViewRows() {
    WTreeView& tv = *ui->treeView;

    // Handle delete action
    QSqlDatabase db = QSqlDatabase::database("closca");

    // Get the selected rows from the view
    QModelIndexList selectedRows = tv.selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tNo rows selected for deletion.";
        return false;
    }

    // Start a transaction
    if (!db.transaction()) {
        qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tFailed to start database transaction.";
        return false;
    }

    bool allSuccess = true;
    QString tableName = m_activeModel->getTableMap().firstKey();
    QString pkColumn = m_activeModel->fieldName(0).toString();

    // Iterate through the selected rows and delete each one
    foreach (const QModelIndex &index, selectedRows) {
        // Get the primary key value for each selected row
        QVariant pk = m_proxyModel->data(m_proxyModel->index(index.row(), 0), Qt::EditRole);
        qDebug() << index.row();
        // Prepare the delete query for the current row
        QSqlQuery query(db);
        query.prepare(QString("DELETE FROM %1 WHERE %2 = :pk").arg(tableName).arg(pkColumn));
        query.bindValue(":pk", pk);

        // Execute the query
        if (!query.exec()) {
            qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tFailed to delete entry with PK" << pk << ":" << query.lastError().text();
            allSuccess = false;
            break;  // Stop the loop on failure
        }
    }

    // Commit or rollback based on success
    if (allSuccess) {
        if (!db.commit()) {
            qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tFailed to commit the transaction.";
        } else {
            // Remove the row from the proxy model
            qDebug() << m_proxyModel->removeRows(selectedRows.first().row(), selectedRows.count());
            qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tEntries deleted successfully.";
            return true;
        }
    } else {
        db.rollback();
        qDebug() << "WDbView::removeSelectedTreeViewRows: \n\tTransaction rolled back due to an error.";
    }
    return false;
}

void WDbView::showTreeContextMenu(const QPoint &point) {
    WTreeView& tv = *ui->treeView;

    // Create context menu
    QMenu contextMenu(&tv);

    // Create actions
    QAction *action_deleteRows = contextMenu.addAction("Delete Selected Entries");
    QAction *action_downloadPdf = nullptr; // Only for Utility Bills table

    // Check if the active model is "Utility Bills"
    if(m_activeModel == CDbConnection::getConnection().getModel("Utility Bills")) {
        // Add "Download selected bills as PDF" action only for Utility Bills
        action_downloadPdf = contextMenu.addAction("Download selected bills as PDF");
    }

    // Show the context menu at the cursor position
    QAction *selectedAction = contextMenu.exec(tv.viewport()->mapToGlobal(point));

    // Check which action was selected
    if (selectedAction == action_downloadPdf) {
        // Get the selected rows from the tree view
        QModelIndexList selectedRows = tv.selectionModel()->selectedRows();

        // Loop through all selected rows
        foreach (const QModelIndex &index, selectedRows) {
            // Trigger the PDF file download for the current row
            reconstructPDFFile(index);
        }
    } else if (selectedAction == action_deleteRows) {
        removeSelectedTreeViewRows();
    }
}
