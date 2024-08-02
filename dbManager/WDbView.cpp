#include "WDbView.h"
#include "ui_WDbView.h"

#include "DMemberEdit.h"
#include "WSearcherHeader.h"
#include "CDbConnection.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlIndex>


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
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Make connections to delete searchBoxes and create new ones everytime the header data changes
    connect(m_proxyModel, &QAbstractTableModel::modelReset, m_head, &WSearcherHeader::updateHeader);
    connect(m_head, &WSearcherHeader::filterChanged, this, &WDbView::filterChanged);

    changeTable("Members");
}

void WDbView::changeTable(const QString& tableName) {
    // Sets the new table, performs the select statement and hides/shows columns custimized
    m_activeModel = CDbConnection::getConnection().getModel(tableName);
    m_proxyModel->setSourceModel(m_activeModel);

    if(tableName == "occupancy_view") {
        ui->treeView->hideColumn(1);
        ui->treeView->hideColumn(2);
        ui->treeView->hideColumn(3);
    }
    else {
        ui->treeView->showColumn(1);
        ui->treeView->showColumn(2);
        ui->treeView->showColumn(3);
    }
    m_activeModel->select();
}

WDbView::~WDbView() {
    delete ui;
}

void WDbView::on_comboBox_tables_currentIndexChanged(int index) {
    // Changing the comboBox table name re-queries to display the selected
    // table info. Each element has its own pre-defined query.
    insertionOrder.clear();
    switch (index) {
    case 0: // Table: members
        changeTable("Members");
        break;

    case 1: // Table: flats
        changeTable("Flats");
        break;

    case 3: // Table Occupancy
        changeTable("Occupancy");

    default:
        break;
    }
}

bool WDbView::openEditDlg(const QModelIndex &index) {
    // Opens a dialog that displays the selected row info and allows updating
    if (!index.isValid() || !m_proxyModel) return false;

    // Retrieve proxy mapped index
    QModelIndex proxyIndex = m_proxyModel->mapToSource(index);

    // Create and show the dialog
    DMemberEdit *dialog = new DMemberEdit(m_activeModel, proxyIndex, this);
    dialog->exec();

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
