/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CParsedFileModel.h"

#include "CParsedFile.h"
#include "QVector"
#include "document/CExportCSV.h"

CParsedFileModel::CParsedFileModel(CExportCSV &relatedExpCSV, QObject *parent)
    : QAbstractItemModel{parent}, m_relExpCSV{relatedExpCSV} {}

int CParsedFileModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_relExpCSV.getFiles().size();
}

int CParsedFileModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_relExpCSV.getFiles().isEmpty() ? 0 : 1;
}

QVariant CParsedFileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return m_relExpCSV.getFiles().at(index.row()).fileName();
    } else if(role == Qt::UserRole) {
        const QHash<QString, QString>& fields = m_relExpCSV.getFiles().at(index.row()).getFields();
        QVector<QString> keys = QVector<QString>::fromList(fields.keys());
        return QVariant::fromValue(keys);

    } else if(role == Qt::UserRole + 1) {
        const QHash<QString, QString>& fields = m_relExpCSV.getFiles().at(index.row()).getFields();
        const QVector<QString>& values = QVector<QString>::fromList(fields.values());
        return QVariant::fromValue(values);
    }

    return QVariant();
}

void CParsedFileModel::appendData(const QString &value) {
    int newRow = rowCount(); // Get the index for the new row

    beginInsertRows(QModelIndex(), newRow, newRow); // Notify the view about the new row

    m_relExpCSV.addFileFromPath(value); // Add the new data to your underlying data structure
    if(!m_relExpCSV.parseFileValues(m_relExpCSV.getFiles().size() - 1)) { // Parse last file (the one we just added);
        qWarning() << "Couldn't parse file";
    }
    endInsertRows(); // Finalize the insertion
}

bool CParsedFileModel::removeData(const QModelIndexList &selectedIndexes) {
    if (selectedIndexes.isEmpty()) {
        return false;
    }

    // Sort indexes in descending order to avoid invalidating indexes when removing rows
    QList<int> rows;
    foreach (const QModelIndex &index, selectedIndexes) {
        if (index.isValid() && index.parent() == QModelIndex()) { // Ensure it's a valid index and in the correct parent
            rows.append(index.row());
        }
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());

    if (rows.isEmpty()) {
        return false;
    }

    // Notify the view that rows are about to be removed
    beginRemoveRows(QModelIndex(), rows.last(), rows.first());

    // Remove the rows from the underlying data structure
    for (int row : rows) {
        m_relExpCSV.deleteFile(row); // Implement this method to handle data removal
    }

    // Notify the view that the rows have been removed
    endRemoveRows();

    return true;
}

QModelIndex CParsedFileModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex CParsedFileModel::parent(const QModelIndex &index) const {
    Q_UNUSED(index);
    return QModelIndex();
}

QVariant CParsedFileModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return QString("Column %1").arg(section + 1);
        } else {
            return QString("Row %1").arg(section + 1);
        }
    }

    return QVariant();
}
