/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CPARSEDPDFMODEL_H
#define CPARSEDPDFMODEL_H

#include "CParsedFile.h"
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

        if (row < 0 || row > m_data.size() || count <= 0) {
            return false; // Invalid row index or invalid count
        }

        QVector<QVariant> emptyRow(m_columnCount, QVariant()); // Initialize the row with empty values

        beginInsertRows(parent, row, row + count - 1); // Ensure that (row + count - 1) >= row
        for (int i = 0; i < count; ++i) {
            m_data.insert(row, qMakePair(nullptr, emptyRow));
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
            return m_data.at(row).second.at(column);

        }

        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (!index.isValid() || index.row() >= m_data.size() || index.column() >= m_columnCount) {
            return false;
        }

        int row = index.row();
        int column = index.column();

        if (role == Qt::EditRole) {
            // Update cell value
            m_data[row].second[column] = value;

            // Notify views that the data has changed
            emit dataChanged(index, index);
            return true;
        }

        return false;
    }

    const CParsedFile* getFileMetadata(const int row) const {
        if(row > m_data.size() || row < 0) return nullptr;
        return m_data[row].first;

    }

    bool setFileMetadata(const int row, const CParsedFile* data) {
        if(row > m_data.size() || row < 0) return false;
        m_data[row].first = data;
        return true;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override{
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override {
        if (role != Qt::EditRole || section < 0) {
            return false;
        }

        if (orientation == Qt::Horizontal) {
            // Handle horizontal header
            if (section >= m_hHeaderData.size()) {
                // Resize if section index is out of bounds
                m_hHeaderData.resize(section + 1);
            }
            m_hHeaderData[section] = value; // Store the value for the horizontal header
        } else if (orientation == Qt::Vertical) {
            // Handle vertical header
            if (section >= m_vHeaderData.size()) {
                // Resize if section index is out of bounds
                m_vHeaderData.resize(section + 1);
            }
            m_vHeaderData[section] = value; // Store the value for the vertical header
        } else {
            return false; // Invalid orientation
        }

        emit headerDataChanged(orientation, section, section);

        return true;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole) {
            return QVariant(); // Only return data for the DisplayRole
        }

        if (section < 0) {
            return QVariant(); // Invalid section index
        }

        if (orientation == Qt::Horizontal) {
            // Return data for horizontal header
            if (section < m_hHeaderData.size()) {
                return m_hHeaderData[section];
            }
        } else if (orientation == Qt::Vertical) {
            // Return data for vertical header
            if (section < m_vHeaderData.size()) {
                return m_vHeaderData[section];
            }
        }

        // No data is available. Return the section index (row or column number)
        return section + 1;
    }

private:
    QVector<QPair<const CParsedFile*, QVector<QVariant>>> m_data; // Flat table of data with file path related to each row

    QVector<QVariant> m_hHeaderData; // Horizontal header data
    QVector<QVariant> m_vHeaderData; // Vertical header data

    int m_columnCount = 0; // Column count
};

#endif // CPARSEDPDFMODEL_H
