#ifndef CHORIZONTALPROXYMODEL_H
#define CHORIZONTALPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>

class CHorizontalProxyModel : public QAbstractProxyModel {
public:
    CHorizontalProxyModel(QObject* parent = nullptr)
        : QAbstractProxyModel(parent) {}

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override {
        return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
    }

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override {
        return index(sourceIndex.column(), sourceIndex.row());
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return sourceModel() ? sourceModel()->columnCount() : 0;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return sourceModel() ? sourceModel()->rowCount() : 0;
    }

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const override {
        if (!proxyIndex.isValid())
            return QVariant();

        QModelIndex sourceIndex = mapToSource(proxyIndex);
        return sourceModel()->data(sourceIndex, role);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal) {
            return sourceModel()->headerData(section, Qt::Vertical, role);
        } else {
            return sourceModel()->headerData(section, Qt::Horizontal, role);
        }
    }

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex& child) const override {
        Q_UNUSED(child);
        return QModelIndex();
    }
};

#endif // CHORIZONTALPROXYMODEL_H
