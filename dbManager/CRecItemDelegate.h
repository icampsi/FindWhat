/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CRECITEMDELEGATE_H
#define CRECITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

class CRecItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CRecItemDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void disableCustomWidgets(bool disable) { m_disableCustomWidgets = disable; }

private:
    bool m_disableCustomWidgets; // If m_disableCustomWidgets is set we will only return line edits.
};

#endif // CRECITEMDELEGATE_H
