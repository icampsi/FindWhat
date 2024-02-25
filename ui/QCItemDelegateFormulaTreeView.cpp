#include "QCItemDelegateFormulaTreeView.h"

QCItemDelegateFormulaTreeView::QCItemDelegateFormulaTreeView(QObject *parent) : QStyledItemDelegate(parent) {}

void QCItemDelegateFormulaTreeView::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // Call the base class paint method to paint the standard item decoration
    QStyledItemDelegate::paint(painter, option, index);

    // Custom painting for parent items
    if (!index.parent().isValid()) {
        // Fill background
        painter->fillRect(option.rect, QColor(169, 169, 169)); // Light gray background color for parent items
        // Draw item text
        painter->setPen(option.palette.color(QPalette::Text)); // Set text color
        painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
    }
    else {
        // HOVER AND SELECT CHILD ITEMS
        if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
            // Set custom background color when selected or hovered
            painter->fillRect(option.rect, QColor(245, 169, 34)); // Light gray background color
            painter->setPen(option.palette.color(QPalette::HighlightedText)); // Set text color
            painter->setPen(option.palette.color(QPalette::Text)); // Set text color
            painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
        }
    }
}

QWidget *QCItemDelegateFormulaTreeView::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
    if (editor) {
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor)) {
            connect(lineEdit, &QLineEdit::editingFinished, parent, [index, lineEdit, this]() {
                emit editingFinished(index, lineEdit->text());
            });
        }
    }
    return editor;
}
