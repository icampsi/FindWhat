/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CSQLMULTITABLEMODEL_H
#define CSQLMULTITABLEMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>
/*
 * This model is specialized on displaying and editing records from complex querys. Is not fast for big data sources
 * since to acomplish the complex edition it keeps track to the table related to every cell and then queryes each one
 * individually. It has a Single record setup where you can display a single record individually for easy query.
 * No sql operations or aliases should be used on the SELECT query, otherwise it can't edit the cell.
 * It's also heavier than models based on QSqlQuery since this model copies the queried data instead of using the
 * fetching-when-needed capabilities of QSqlQuery.
 *
 * Use BehaviourFlag::Insert to display an empty record that you can then use to insert a new record instead of
 * just updating an existing one.
*/

class CSqlMultiTableModel : public QAbstractTableModel { // BOOKMARK - Should rename it to CSqlMultiTableModel
    Q_OBJECT

private:
    class Field {
        // INTERNAL DATA STRUCTURES
    public:
        Field(const QString& fieldName, bool hasFk = false, const QString&fkTableName = "", const QString& fkColumnName = "")
            : m_fieldName{fieldName}, m_hasFk{hasFk}, m_fkTableName{fkTableName}, m_fkColumnName{fkColumnName} {
        }

        QString          m_fieldName      = "";

        bool             m_hasFk          = false;
        QString          m_fkTableName    = "";
        QString          m_fkColumnName   = "";
        QVector<QString> m_fkColumnValues;
    };

public:
    // CONSTURCTORS & DESTRUCTORS
    explicit CSqlMultiTableModel(QObject *parent = nullptr, int index = 0, QString query = "", QSqlDatabase db = QSqlDatabase());
    ~CSqlMultiTableModel() {}

    // PUBLIC METHODS
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant fieldName(int index) const; // Returns field name from index

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Change the referenced record for edition based on row index of the query results
    void changeRecord(int index);

    void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
    void setQuery(QSqlQuery &&query);

    // Query again in order to update data.
    void requery(const QSqlDatabase &db = QSqlDatabase::database("closca")) { setQuery(m_query.lastQuery(), db); }

    // Parse field names to change _ and camel letters for spaces
    QString processFieldName(const QString &fieldName) const;

    // Binds values to the current update querries and submits the record on the database
    bool commitRecord(const int row);
    bool commitTable();

    // Appends empty records
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Made Non-functional
    bool insertColumns(int col, int count, const QModelIndex &parent = QModelIndex()) override {
        Q_UNUSED(col); Q_UNUSED(count); Q_UNUSED(parent);
        return false;
    }

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Retrieves the string used in setQuery();
    const QString lastQuery() const { return this->m_query.lastQuery(); }

    const QMap<QString, QVector<Field>>& getTableMap() { return m_tables; }

protected:
    // Gets the different table names used from the db and maps them to their fields: m_tables
    void extractTables();
    /*
     * Automatically creates an update query for every table referenced on the model
     * and every field on this table.
    */
    void formUpsertQuery();

    // // Properties that would make a field value non editable.
    // bool editableCheck(const QModelIndex &index) const;

    Field retrieveForeignKeyInfo(const QString& tableName, const QString& columnName);

    // The shared part of the setQuery function for each overload
    void doQuery();

    // - NOT USED YET
    // void CRecModel::copyRecordsToVector() {
    //     m_recordCache.clear();  // Clear existing records
    //     for (int i = 0; i < rowCount(); ++i) {
    //         m_recordCache.append(new QSqlRecord(record(i)));
    //     }
    // }
    // QVector<QSqlRecord*> m_recordCache;
    // on destructor:     qDeleteAll(m_recordCache);

signals:
    void cellValueChanged(int row); // emit when a field is edited

    // MEMBERS
private:
    QMap<QString, QVector<Field>>   m_tables; // Map tableName->it's fields
    QVector<QSqlQuery>              m_updateQuerys;
    QVector<QSqlRecord>             m_records;
    QSqlQuery                       m_query;
    int                             m_activeRecordIndex;
    /*
     * Used to set the behaviour of the model as an update model (that will display an existing record based on m-.recordIndex)
     * or as an Insert model (which would display a newly generated empty record for you to insert)
    */
    int                             m_rowCount;
};

#endif // CSQLMULTITABLEMODEL_H
