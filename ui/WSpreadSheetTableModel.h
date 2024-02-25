#include <QApplication>
#include <QAbstractTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QPushButton>

class WSpreadSheetTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    WSpreadSheetTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_rows;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_columns;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole)
            return QString("Row %1, Column %2").arg(index.row() + 1).arg(index.column() + 1);
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    void addRow() {
        ++m_rows;
        emit layoutChanged();
    }

    void addColumn() {
        ++m_columns;
        emit layoutChanged();
    }

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        beginInsertRows(parent, row, row + count - 1);
        m_rows += count;
        endInsertRows();
        return true;
    }

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override {
        beginInsertColumns(parent, column, column + count - 1);
        m_columns += count;
        endInsertColumns();
        return true;
    }

private:
    int m_rows = 4; // Initial rows
    int m_columns = 4; // Initial columns
};
