/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "DMemberEdit.h"

#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QSqlError>

DMemberEdit::DMemberEdit(const QString& query, const QSqlDatabase& db, const QModelIndex &modelIndex, QWidget *parent)
    : QDialog(parent), m_index{modelIndex}, m_recModel(this, modelIndex.row(), query, db), m_hProxyModel(this)
{
    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(Layout);

    // Model and proxy
    m_recModel.setMode(CSqlMultiTableModel::Mode::SingleRecord);
    m_hProxyModel.setSourceModel(&m_recModel);
    // Initialize the table widget
    m_table = new WSqlMultiTable(this);
    m_table->setModel(&m_hProxyModel);

    m_recModel.setHeaderData(0, Qt::Horizontal, "Field");
    m_recModel.setHeaderData(1, Qt::Horizontal, "Value");

    // Add the table to layout
    Layout->addWidget(m_table);
    m_table->show();

    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DMemberEdit::submit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    Layout->addWidget(buttonBox);

    // Calculate the appropriate maximum height for the table
    int rowCount = m_hProxyModel.rowCount();

    int rowHeight       = m_table->verticalHeader()->defaultSectionSize();
    int headerHeight    = m_table->horizontalHeader()->height();
    int maxTableHeight  = rowHeight * rowCount + headerHeight;
    int maxWindowHeight = maxTableHeight + buttonBox->height() + 24;

    setMinimumSize(QSize(178, 287));
    setMaximumSize(QSize(330, 287));

    setFixedHeight(maxWindowHeight);
    setMaximumWidth(330);
}

DMemberEdit::~DMemberEdit() {
    delete m_table;
}

bool DMemberEdit::updateRecord() {
    // Submit changes to the database
    if(m_recModel.submitRecord()) {
        qDebug() << "Row modified successfully!";
        return true;
    } else {
        qDebug() << "Error modifying row:" << m_recModel.lastError().text();
        return false;
    }
}

void DMemberEdit::submit() {
    if(updateRecord()) accept();
    else qDebug() << "Couldn't update record";
}
