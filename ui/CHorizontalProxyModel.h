/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CHORIZONTALPROXYMODEL_H
#define CHORIZONTALPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>

/*
 * This Proxy Model has the only function of changing the view direction,
 * so rows become columns and columns become rows.
*/

class CHorizontalProxyModel : public QAbstractProxyModel {
public:
    CHorizontalProxyModel(QObject* parent = nullptr);

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
};

#endif // CHORIZONTALPROXYMODEL_H
