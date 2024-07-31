/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WRECEDITTABLE_H
#define WRECEDITTABLE_H

#include <QSqlTableModel>
#include <QObject>
#include <QWidget>
#include <QTableView>

#include <QAbstractTableModel>
#include <QSqlTableModel>
#include <QVector>
#include <QPair>

class CSqlRecordModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit CSqlRecordModel(QSqlTableModel *sourceModel = nullptr, int mirrorRowIndex = -1, QObject *parent = nullptr);

    void updateFields(QSqlTableModel *sourceModel, int row = -1);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return fieldPairs.size(); // One row for each field
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 2; // "Field" and "Value"
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant value(int index, int role = Qt::DisplayRole) const;
    QVariant fieldName(int index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

signals:
    void cellValueChanged(int row); // emit when a field is edited

private:
    QVector<QPair<QString, QString>> fieldPairs; // Pairs of field names and values
};

class WRecEditTable : public QTableView {
    Q_OBJECT

public:
    explicit WRecEditTable(QWidget* parent = nullptr);

    const QString value(int index) const { return sqlRecordModel()->value(index).toString(); }

    // Custom setter that accepts only CSqlRecordModel
    void setSqlRecordModel(CSqlRecordModel *model);

    // Getter that returns CSqlRecordModel*
    CSqlRecordModel* sqlRecordModel() const { return dynamic_cast<CSqlRecordModel*>(QTableView::model()); }

private:
    // hide setModel so only setSqlRecordModel is allowed
    void setModel(QAbstractItemModel *model) override;
};

#endif // WRECEDITTABLE_H
