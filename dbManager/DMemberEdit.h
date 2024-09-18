/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef DMEMBEREDIT_H
#define DMEMBEREDIT_H

#include <QSqlRelationalTableModel>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QDialog>

#include "WSqlMultiTable.h"
#include "ui/CHorizontalProxyModel.h"

class DMemberEdit : public QDialog {
    Q_OBJECT

public:
    explicit DMemberEdit(CSqlMultiTableModel *sourceModel, const int rowToEdit, QWidget *parent = nullptr);
    ~DMemberEdit();

private:
    bool updateRecord();

private slots:
    void submit();

private:
    WSqlMultiTable          *m_table;
    CSqlMultiTableModel     *m_recModel;
    CHorizontalProxyModel    m_hProxyModel;
    int                      m_rowIndex;
};

#endif // DMEMBEREDIT_H
