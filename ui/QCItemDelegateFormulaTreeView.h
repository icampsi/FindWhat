#ifndef QCITEMDELEGATEFORMULATREEVIEW_H
#define QCITEMDELEGATEFORMULATREEVIEW_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QLineEdit>

class QCItemDelegateFormulaTreeView : public QStyledItemDelegate {
    Q_OBJECT
signals:
    void editingFinished(const QModelIndex &index, const QString &text) const;

public:
    explicit QCItemDelegateFormulaTreeView(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // QCITEMDELEGATEFORMULATREEVIEW_H
