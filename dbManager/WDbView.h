/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WDBVIEW_H
#define WDBVIEW_H

#include "WSearcherHeader.h"

#include <QSortFilterProxyModel>
#include <QWidget>

class CSqlMultiTableModel;

namespace Ui { class WDbView; }

class WDbView : public QWidget {
    Q_OBJECT

public:
    explicit WDbView(QWidget *parent = nullptr);
    ~WDbView();

    void changeTable(const QString& tableName);

protected:
    void reconstructPDFFile(const QModelIndex &index);
    bool removeSelectedTreeViewRows();

private slots:
    void on_comboBox_tables_currentIndexChanged(int index);
    void on_treeView_doubleClicked(const QModelIndex &index);
    bool openEditDlg(const QModelIndex &index);
    void filterChanged(int column, const QString& text);
    void showTreeContextMenu(const QPoint &point);

signals:
    void queryChanged();

protected:
    Ui::WDbView             *ui;

    QSortFilterProxyModel   *m_proxyModel;
    WSearcherHeader         *m_head;
    QVector<QString>         m_insertionOrder;

    CSqlMultiTableModel     *m_activeModel;
};

#endif // WDBVIEW_H
