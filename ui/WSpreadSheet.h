/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WSPREADSHEET_H
#define WSPREADSHEET_H

#include <QApplication>
#include <QClipboard>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QKeyEvent>
#include <QItemSelectionModel>
#include <QMenu>

class WSpreadSheet : public QTableView {
    Q_OBJECT

public:
    WSpreadSheet(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void HFit();
    void VFit();

private slots:
    void showContextMenu(const QPoint &pos);
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();
    void deleteSelectedCells();

    void addRow();
    void insertRow(bool before, int row);
    void delSelectedRows();
    void addCol();
    void insertCol(bool before, int col);
    void delSelectedColumns();

    void showRowHeaderContextMenu(const QPoint &pos);    // Slot to show row header context menu
    void showColumnHeaderContextMenu(const QPoint &pos); // Slot to show column header context menu


public:
    QMenu *rowHeaderContextMenu;
    QMenu *columnHeaderContextMenu;
};

#endif // WSPREADSHEET_H
