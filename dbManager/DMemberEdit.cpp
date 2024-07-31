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

DMemberEdit::DMemberEdit(QSqlTableModel *model, const QModelIndex &modelIndex,  QWidget *parent)
    : QDialog(parent), m_model{model}, m_index{modelIndex}, m_recModel(model, modelIndex.row(), this)
{
    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(Layout);

    // Initialize the table widget
    m_table = new WRecEditTable(this);
    m_table->setSqlRecordModel(&m_recModel);

    // Add the table to layout
    Layout->addWidget(m_table);
    m_table->show();

    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DMemberEdit::submit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    Layout->addWidget(buttonBox);

    // Calculate the appropriate maximum height for the table
    QSqlRecord rec = model->record(m_index.row());
    int rowCount = rec.count();

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
    updateModel();
    // Submit changes to the database
    if (m_model->submitAll()) {
        qDebug() << "Row modified successfully!";
        return true;
    } else {
        qDebug() << "Error modifying row:" << m_model->lastError().text();
        return false;
    }
}

void DMemberEdit::updateModel() {
    QAbstractItemModel *model = m_table->model();

    if (model) {
        for (int row = 0; row < model->rowCount(); ++row) {
                QVariant value = m_table->value(row);
                QModelIndex i = m_model->index(m_index.row(), row);
                m_model->setData(i, value);
        }
    }
}

void DMemberEdit::submit() {
    if(updateRecord()) accept();
    else qDebug() << "Coulnd't update record";
}
