/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CRecItemDelegate.h"

#include <QComboBox>
#include <QLineEdit>

CRecItemDelegate::CRecItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

QWidget* CRecItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // Check the custom role to see if this cell should use a QComboBox
    QVariant comboBoxData = index.model()->data(index, Qt::UserRole + 1);

    if (comboBoxData.isValid()) {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItems(comboBoxData.toStringList());
        qDebug() << "USED";
        return comboBox;
    }

    // Default to QLineEdit or other default widget
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void CRecItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (QComboBox *comboBox = qobject_cast<QComboBox*>(editor)) {
        comboBox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CRecItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (QComboBox *comboBox = qobject_cast<QComboBox*>(editor)) {
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
