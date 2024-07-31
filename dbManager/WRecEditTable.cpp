/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WRecEditTable.h"

#include <QHeaderView>

CSqlRecordModel::CSqlRecordModel(QSqlTableModel *sourceModel, int mirrorRowIndex, QObject *parent)
    : QAbstractTableModel(parent) {
    updateFields(sourceModel, mirrorRowIndex);
}

void CSqlRecordModel::updateFields(QSqlTableModel *sourceModel, int row) {
    if (sourceModel) {
        fieldPairs.clear(); // Clear any existing data

        // Get the number of columns from the source model
        int columnCount = sourceModel->columnCount();
        for (int i = 0; i < columnCount; ++i) {
            QString fieldName = sourceModel->headerData(i, Qt::Horizontal).toString();
            QString value; // Default to empty value

            if (row >= 0 && row < sourceModel->rowCount()) {
                // Fetch the value from the specified row if it's valid
                QModelIndex index = sourceModel->index(row, i);
                value = sourceModel->data(index).toString();
            }

            fieldPairs.append(qMakePair(fieldName, value));
        }

        // Notify that the data has changed
        beginResetModel();
        endResetModel();
    }
}

QVariant CSqlRecordModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        const int row = index.row();
        const int col = index.column();

        if (row >= 0 && row < fieldPairs.size()) {
            if (col == 0) {
                return fieldPairs.at(row).first; // Field name
            } else if (col == 1) {
                return fieldPairs.at(row).second; // Field value
            }
        }
    } else if (role == Qt::EditRole) {
        // Provide the value for editing
        const int row = index.row();
        const int col = index.column();

        if (row >= 0 && row < fieldPairs.size()) {
            if (col == 1) {
                return fieldPairs.at(row).second; // Field value
            }
        }
    }

    return QVariant();
}

QVariant CSqlRecordModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return "Field";
        else if (section == 1)
            return "Value";
    }
    return QVariant();
}

bool CSqlRecordModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    const int row = index.row();
    const int col = index.column();

    if (row >= 0 && row < fieldPairs.size() && col == 1) {
        // Update the value in the fieldPairs
        fieldPairs[row].second = value.toString();

        // Notify that the data has changed
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        emit cellValueChanged(row);
        return true;
    }

    return false;
}

Qt::ItemFlags CSqlRecordModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.column() == 0) {
        return defaultFlags & ~Qt::ItemIsEditable; // Non-editable
    }

    return defaultFlags | Qt::ItemIsEditable; // Editable
}

QVariant CSqlRecordModel::value(int index, int role) const {
    if (index < 0 || index >= fieldPairs.size() || role != Qt::DisplayRole) {
        return QVariant();
    }
    return fieldPairs.at(index).second; // Field values
}

QVariant CSqlRecordModel::fieldName(int index, int role) const {
    if (index < 0 || index >= fieldPairs.size() || role != Qt::DisplayRole) {
        return QVariant();
    }
    return fieldPairs.at(index).first; // Field names
}

// -----------------------------------------------------------------------------------------------------

WRecEditTable::WRecEditTable(QWidget *parent)
    : QTableView(parent)
{
    // CONFIGURATION
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void WRecEditTable::setSqlRecordModel(CSqlRecordModel *model) {
    if (model) {
        // Set the model using the base class method
        QTableView::setModel(model);
    } else {
        qWarning() << "Error: Attempted to set a null CSqlRecordModel.";
    }
}

void WRecEditTable::setModel(QAbstractItemModel *model) {
    if (dynamic_cast<CSqlRecordModel*>(model)) {
        // If the model is of type CSqlRecordModel, set it
        QTableView::setModel(model);
    } else {
        // Log an error or handle it as needed
        qWarning() << "Error: Only CSqlRecordModel is accepted.";
    }
}
