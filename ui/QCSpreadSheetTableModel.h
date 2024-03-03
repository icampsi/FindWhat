#include <QApplication>
#include <QAbstractTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QPushButton>

class QCSpreadSheetTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    QCSpreadSheetTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override { return QAbstractTableModel::flags(index) | Qt::ItemIsEditable; }

    void addRow();
    void addColumn();
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    int m_rows = 4;    // Initial rows
    int m_columns = 4; // Initial columns
};
