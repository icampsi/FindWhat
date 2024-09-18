/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WSQLMULTITABLE_H
#define WSQLMULTITABLE_H

#include <QWidget>
#include <QTableView>

#include "CSqlMultiTableModel.h"

    // Simple subclass with the hardcoded format i feel the QSqlMultiTableModel should have when in singleRecord mode.
class WSqlMultiTable : public QTableView {
    Q_OBJECT

public:
    explicit WSqlMultiTable(QWidget* parent = nullptr);

    // GETTERS
    CSqlMultiTableModel* sqlRecordModel() const { return dynamic_cast<CSqlMultiTableModel*>(QTableView::model()); }
    // SETTERS
    void setModel(QAbstractItemModel *model) override;
};

#endif // WSQLMULTITABLE_H
