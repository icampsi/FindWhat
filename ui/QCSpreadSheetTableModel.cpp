#include "QCSpreadSheetTableModel.h"


int QCSpreadSheetTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_rows;
}

int QCSpreadSheetTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_columns;
}

QVariant QCSpreadSheetTableModel::data(const QModelIndex &index, int role) const  {
    if (role == Qt::DisplayRole)
        return QString("Row %1, Column %2").arg(index.row() + 1).arg(index.column() + 1);
    return QVariant();
}

void QCSpreadSheetTableModel::addRow() {
    ++m_rows;
    emit layoutChanged();
}

void QCSpreadSheetTableModel::addColumn() {
    ++m_columns;
    emit layoutChanged();
}

bool QCSpreadSheetTableModel::insertRows(int row, int count, const QModelIndex &parent)  {
    beginInsertRows(parent, row, row + count - 1);
    m_rows += count;
    endInsertRows();
    return true;
}

bool QCSpreadSheetTableModel::insertColumns(int column, int count, const QModelIndex &parent) {
    beginInsertColumns(parent, column, column + count - 1);
    m_columns += count;
    endInsertColumns();
    return true;
}
