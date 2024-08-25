/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WRECEDITTABLE_H
#define WRECEDITTABLE_H

#include <QWidget>
#include <QTableView>

#include "CRecModel.h"

class WRecEditTable : public QTableView {
    Q_OBJECT

public:
    explicit WRecEditTable(QWidget* parent = nullptr);

    // GETTERS
    const QString value(int index) const { return sqlRecordModel()->value(index).toString(); }
    CRecModel* sqlRecordModel() const { return dynamic_cast<CRecModel*>(QTableView::model()); }
    // SETTERS
    void setSqlRecordModel(CRecModel *model);

private:
    // hide setModel so only setSqlRecordModel is allowed
    void setModel(QAbstractItemModel *model) override;
};

#endif // WRECEDITTABLE_H
