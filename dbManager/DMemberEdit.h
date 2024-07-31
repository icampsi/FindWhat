/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef DMEMBEREDIT_H
#define DMEMBEREDIT_H

#include <QSqlTableModel>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QDialog>

#include "WRecEditTable.h"

class DMemberEdit : public QDialog {
    Q_OBJECT

public:
    explicit DMemberEdit(QSqlTableModel *model, const QModelIndex &modelIndex, QWidget *parent = nullptr);
    ~DMemberEdit();

private:
    bool updateRecord();
    void updateModel(); // Updates the original model with all the field from m_table

private slots:
    void submit();

private:
    WRecEditTable     *m_table;
    QSqlTableModel    *m_model;
    const QModelIndex &m_index;

    CSqlRecordModel m_recModel;
};

#endif // DMEMBEREDIT_H
