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

class DMemberEdit : public QDialog {
    Q_OBJECT

public:
    explicit DMemberEdit(const QString& query, const QSqlDatabase& db, const QModelIndex &modelIndex, QWidget *parent = nullptr);
    ~DMemberEdit();

private:
    bool updateRecord();

private slots:
    void submit();

private:
    WSqlMultiTable               *m_table;
    const QModelIndex           &m_index;
    
    CSqlMultiTableModel                   m_recModel;
};

#endif // DMEMBEREDIT_H
