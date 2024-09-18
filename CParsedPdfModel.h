#ifndef CPARSEDPDFMODEL_H
#define CPARSEDPDFMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QPair>

class CParsedPdfModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit CParsedPdfModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent) {}

    // Re-implement rowCount
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();
    }

    // Re-implement columnCount (optional, depends on your data structure)
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);

        return m_columnCount;
    }

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override {
        Q_UNUSED(parent);

        if (column < 0 || column > m_columnCount || count <= 0) {
            return false; // Invalid column index or invalid count
        }

        beginInsertColumns(parent, column, column + count - 1);
        for (int i = 0; i < m_data.size(); ++i) {
            // Insert empty columns into each row's data
            for (int j = 0; j < count; ++j) {
                m_data[i].second.insert(column, QVariant());
            }
        }
        m_columnCount += count;
        endInsertColumns();

        return true;
    }

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        Q_UNUSED(parent);

        if (row < 0 || row >= m_data.size() || count <= 0) {
            return false; // Invalid row index or invalid count
        }

        QVector<QVariant> emptyRow(m_columnCount, QVariant()); // Initialize the row with empty values

        beginInsertRows(parent, row, row + count - 1); // Ensure that (row + count - 1) >= row
        for (int i = 0; i < count; ++i) {
            m_data.insert(row, qMakePair(QString(), emptyRow));
        }
        endInsertRows();

        return true;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size())
            return QVariant();
        int row = index.row();
        int column = index.column();
        if (role == Qt::DisplayRole) {
            qDebug() << m_data.at(row).second.at(column);
            return m_data.at(row).second.at(column);

        } else if (role == Qt::UserRole) {
            // Return the file path metadata
            return m_data.at(row).first;
        }

        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (!index.isValid() || index.row() >= m_data.size() || index.column() >= m_data.at(index.row()).second.size())
            return false;

        int row = index.row();
        int column = index.column();

        if (role == Qt::EditRole) {
            // Update cell value
            m_data[row].second[column] = value;
            qDebug() << "Value: " << value;
            qDebug() << "cell: " << m_data[row].second[column];
            qDebug() << "Column: " << column;
            qDebug() << "Row: " << row;
            // Notify views that the data has changed
            emit dataChanged(index, index);
            return true;
        } else if( role == Qt::UserRole) { // use userRole to set filePath
            m_data[row].first = value.toString();
            return true;
        }

        return false;
    }

private:
    QVector<QPair<QString, QVector<QVariant>>> m_data; // Flat table of data with file path related to each row
    int m_columnCount = 0;
};

#endif // CPARSEDPDFMODEL_H
