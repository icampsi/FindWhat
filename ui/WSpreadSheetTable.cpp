#include "WSpreadSheetTable.h"
#include "WSpreadSheetTableModel.h"

WSpreadSheetTable::WSpreadSheetTable(QWidget* parent) : QTableView(parent) {
    WSpreadSheetTableModel model;
    setModel(&model);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    QPushButton addRowButton("Add Row");
    QPushButton addColumnButton("Add Column");
    model.addRow();
    model.addColumn();

    QObject::connect(&addRowButton, &QPushButton::clicked, [&model]() {
        model.addRow();
    });

    QObject::connect(&addColumnButton, &QPushButton::clicked, [&model]() {
        model.addColumn();
    });
}
