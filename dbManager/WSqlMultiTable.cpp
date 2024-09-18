/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WSqlMultiTable.h"
#include "CSqlMultiTableModel.h"
#include "CSqlMultiTableDelegate.h"
#include "qabstractproxymodel.h"

#include <QHeaderView>

WSqlMultiTable::WSqlMultiTable(QWidget *parent)
    : QTableView(parent)
{
    // CONFIGURATION
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setVisible(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // Set CSqlMultiTableDelegate as default item delegate
    CSqlMultiTableDelegate *delegate = new CSqlMultiTableDelegate(this);
    setItemDelegate(delegate);
}

void WSqlMultiTable::setModel(QAbstractItemModel *model) {
    if (dynamic_cast<CSqlMultiTableModel*>(model) || dynamic_cast<QAbstractProxyModel*>(model)) {
        // If the model is of type CSqlRecordModel, set it
        QTableView::setModel(model);
        return;
    }
    // Log an error or handle it as needed
    qWarning() << "Error: Only CSqlRecordModel or proxy models with it as source are accepted.";
    return;
}
