/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CHorizontalProxyModel.h"

CHorizontalProxyModel::CHorizontalProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent) {}

QModelIndex CHorizontalProxyModel::mapToSource(const QModelIndex& proxyIndex) const {
    if (!sourceModel() || !proxyIndex.isValid())
        return QModelIndex();
    // Swap row and column
    return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
}

QModelIndex CHorizontalProxyModel::mapFromSource(const QModelIndex& sourceIndex) const {
    if (!sourceIndex.isValid())
        return QModelIndex();
    // Swap row and column back
    return index(sourceIndex.column(), sourceIndex.row());
}

int CHorizontalProxyModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return sourceModel() ? sourceModel()->columnCount() : 0;  // Swap row with column
}

int CHorizontalProxyModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    int sourceRowCount = sourceModel()->rowCount();
    return m_mode == Mode::SingleRow && sourceRowCount > 0 ? 1 : sourceRowCount;  // Handle SingleRow mode
}

QVariant CHorizontalProxyModel::data(const QModelIndex& proxyIndex, int role) const {
    if (!proxyIndex.isValid())
        return QVariant();

    int sourceRow = proxyIndex.row();
    int sourceColumn = m_mode == Mode::SingleRow ? m_targetRow : proxyIndex.column();
    QModelIndex sourceIndex = mapToSource(createIndex(sourceRow, sourceColumn));

    return sourceModel()->data(sourceIndex, role);
}

bool CHorizontalProxyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    int sourceRow = m_mode == Mode::SingleRow ? m_targetRow : index.row();
    int sourceColumn = index.row();

    QModelIndex in = createIndex(sourceRow, sourceColumn);
    return sourceModel()->setData(in, value, role);
}

QVariant CHorizontalProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(!sourceModel()) return QVariant();
    orientation = orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
    if(role == Qt::DisplayRole) {
        return sourceModel()->headerData(section, orientation, role);
    } else {
        return QSortFilterProxyModel::headerData(section, orientation, role);
    }
}

QModelIndex CHorizontalProxyModel::index(int row, int column, const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex CHorizontalProxyModel::parent(const QModelIndex& child) const {
    Q_UNUSED(child);
    return QModelIndex();
}

bool CHorizontalProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    Q_UNUSED(sourceParent);
    return sourceRow == m_targetRow;
}

