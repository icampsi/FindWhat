/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WDBVIEW_H
#define WDBVIEW_H

#include "WSearcherHeader.h"

#include <QSortFilterProxyModel>
#include <QSqlRelationalTableModel>
#include <QWidget>

namespace Ui { class WDbView; }

class WDbView : public QWidget {
    Q_OBJECT

public:
    explicit WDbView(QWidget *parent = nullptr);
    ~WDbView();

    void changeTable(const QString& tableName);

private slots:
    void on_comboBox_tables_currentIndexChanged(int index);
    void on_treeView_doubleClicked(const QModelIndex &index);
    bool openEditDlg(const QModelIndex &index);
    void filterChanged(int column, const QString& text);

signals:
    void queryChanged();

private:
    Ui::WDbView *ui;
    QSortFilterProxyModel *m_proxyModel;
    WSearcherHeader *m_head;
    QVector<QString> insertionOrder;

    QSqlRelationalTableModel *m_activeModel;
};

#endif // WDBVIEW_H
