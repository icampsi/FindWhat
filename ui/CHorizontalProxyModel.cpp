/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CHorizontalProxyModel.h"

CHorizontalProxyModel::CHorizontalProxyModel(QObject* parent)
    : QAbstractProxyModel(parent) {}

QModelIndex CHorizontalProxyModel::mapToSource(const QModelIndex& proxyIndex) const {
    return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
}

QModelIndex CHorizontalProxyModel::mapFromSource(const QModelIndex& sourceIndex) const {
    return index(sourceIndex.column(), sourceIndex.row());
}

int CHorizontalProxyModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return sourceModel() ? sourceModel()->columnCount() : 0;
}

int CHorizontalProxyModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return sourceModel() ? sourceModel()->rowCount() : 0;
}

QVariant CHorizontalProxyModel::data(const QModelIndex& proxyIndex, int role) const {
    if (!proxyIndex.isValid())
        return QVariant();

    QModelIndex sourceIndex = mapToSource(proxyIndex);
    return sourceModel()->data(sourceIndex, role);
}

QVariant CHorizontalProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return sourceModel()->headerData(section, orientation, role);
}

QModelIndex CHorizontalProxyModel::index(int row, int column, const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex CHorizontalProxyModel::parent(const QModelIndex& child) const {
    Q_UNUSED(child);
    return QModelIndex();
}
