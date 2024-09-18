/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CHORIZONTALPROXYMODEL_H
#define CHORIZONTALPROXYMODEL_H

#include <QSortFilterProxyModel>

/*
 * This Proxy Model has the only function of changing the view direction,
 * so rows become columns and columns become rows. Since it is mainly
 * intended to create an easy way to edit single entries on a table,
 * it is also able to switch modes between SingleRow and MultiRow, where
 * it would display only the selected record or the full table.
*/

class CHorizontalProxyModel : public QSortFilterProxyModel {
public:
    enum class Mode { SingleRow, MultiRow };

    CHorizontalProxyModel(QObject* parent = nullptr);

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    void setMode(Mode mode, int targetRow = 0 /*only needed for singleRow mode*/ ) {
        m_mode = mode;
        m_targetRow = targetRow;
    }

    // Override filterAcceptsRow to filter out all rows except the target row
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    Mode m_mode;      // Single row would display only the selected row. Otherwise, full table is displayed.
    int m_targetRow;  // The row number you want to display
};

#endif // CHORIZONTALPROXYMODEL_H
