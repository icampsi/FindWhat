/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WRecEditTable.h"
#include "CRecModel.h"
#include "CRecItemDelegate.h"

#include <QHeaderView>

WRecEditTable::WRecEditTable(QWidget *parent)
    : QTableView(parent)
{
    // CONFIGURATION
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    CRecItemDelegate *delegate = new CRecItemDelegate(this);
    setItemDelegate(delegate);
}

void WRecEditTable::setSqlRecordModel(CRecModel *model) {
    if (model) {
        // Set the model using the base class method
        QTableView::setModel(model);
    } else {
        qWarning() << "Error: Attempted to set a null CSqlRecordModel.";
    }
}

void WRecEditTable::setModel(QAbstractItemModel *model) {
    if (dynamic_cast<CRecModel*>(model)) {
        // If the model is of type CSqlRecordModel, set it
        QTableView::setModel(model);
    } else {
        // Log an error or handle it as needed
        qWarning() << "Error: Only CSqlRecordModel is accepted.";
    }
}
