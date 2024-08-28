// /* =================================================== *
//  * ====        Copyright (c) 2024 icampsi         ==== *
//  * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
//  * =================================================== */

// #ifndef CRECMODEL_H
// #define CRECMODEL_H

// #include <QObject>
// #include <QSqlQueryModel>
// #include <QSqlQuery>

// /*
//  * This model is specialized on displaying a single record on a query displayed in a two vertical column way,
//  * where the left column shows fields and right column values. It is able to display AND UPDATE information
//  * from multiple tables at the same time, by maping its fields with the table they come from.
//  * However, no sql operations or aliases should be used on the SELECT query.
//  *
//  * Use BehaviourFlag::Insert to display an empty record that you can then use to insert a new record instead of
//  * just updating an existing one.
// */

// class CSqlMultiTableModel : public QSqlQueryModel { // BOOKMARK - Should rename it to CSqlMultiTableModel
//     Q_OBJECT

// private:
//     class Field {
//         // INTERNAL DATA STRUCTURES
//     public:
//         Field(const QString& fieldName, bool hasFk = false, const QString&fkTableName = "", const QString& fkColumnName = "")
//             : m_fieldName{fieldName}, m_hasFk{hasFk}, m_fkTableName{fkTableName}, m_fkColumnName{fkColumnName} {
//         }

//         QString          m_fieldName      = "";

//         bool             m_hasFk          = false;
//         QString          m_fkTableName    = "";
//         QString          m_fkColumnName   = "";
//         QVector<QString> m_fkColumnValues;

//         void print() const {
//             qDebug() << "===============\t\t";
//             qDebug() << "Field Name:\t\t"      << m_fieldName;
//             qDebug() << "Has FK:\t\t\t"        << m_hasFk;
//             qDebug() << "FK Table Name:\t\t"   << m_fkTableName;
//             qDebug() << "FK Column Name:\t\t"  << m_fkColumnName;

//             for(const QString& value : m_fkColumnValues) {
//                 qDebug() << "Column value:\t\t" << value;
//             }
//             qDebug() << "\n\n";
//         }
//     };

// public:
//     enum class BehaviourFlag { Update, Insert };

//     // CONSTURCTORS & DESTRUCTORS
//     explicit CSqlMultiTableModel(QObject *parent = nullptr, int index = 0, QString query = "", QSqlDatabase db = QSqlDatabase());
//     ~CSqlMultiTableModel();

//     // PUBLIC METHODS
//     void setBehaviourFlag(BehaviourFlag flag) { m_behaviourFlag = flag;}
//     BehaviourFlag getBehaviourFlag() const { return m_behaviourFlag; }

//     int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//     int columnCount(const QModelIndex &parent = QModelIndex()) const override;

//     QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
//     bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

//     QVariant value(int index, int role = Qt::DisplayRole) const; // Returns value from index
//     QVariant fieldName(int index, int role = Qt::DisplayRole) const; // Returns field name from index

//     QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
//     Qt::ItemFlags flags(const QModelIndex &index) const override;
//     // Change the referenced record for edition based on row index of the query results
//     void changeRecord(int index);

//     void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
//     void setQuery(QSqlQuery &&query);

//     // Parse field names to change _ and camel letters for spaces
//     QString processFieldName(const QString &fieldName) const;

//     // Binds values to the current update querries and submits the record on the database
//     bool submitRecord();

//     // Made Non-functional. Not deleting it in order to avoid troubles when using QAbstractItemModel*
//     bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
//         Q_UNUSED(row); Q_UNUSED(count); Q_UNUSED(parent);
//         return false;
//     }

//     // Made Non-functional. Not deleting it in order to avoid troubles when using QAbstractItemModel*
//     bool insertColumns(int col, int count, const QModelIndex &parent = QModelIndex()) override {
//         Q_UNUSED(col); Q_UNUSED(count); Q_UNUSED(parent);
//         return false;
//     }

// protected:
//     // Retrieves the record information based on the index of m_activeRecordIndex
//     void retrieveRecord();
//     // Gets the different table names used from the db and maps them to their fields: m_tables
//     void extractTables();
//     // Automatically creates an update query for every table referenced on the model
//     // and every field on this table.
//     void formUpsertQuery();

//     // Properties that would make a field value non editable. For now only one is specified:
//     bool editableCheck(const QModelIndex &index) const;

//     Field retrieveForeignKeyInfo(const QString& tableName, const QString& columnName);

//     // - NOT USED YET
//     // void CSqlMultiTableModel::copyRecordsToVector() {
//     //     m_recordCache.clear();  // Clear existing records
//     //     for (int i = 0; i < rowCount(); ++i) {
//     //         m_recordCache.append(new QSqlRecord(record(i)));
//     //     }
//     // }
//     // QVector<QSqlRecord*> m_recordCache;
//     // on destructor:     qDeleteAll(m_recordCache);

// signals:
//     void cellValueChanged(int row); // emit when a field is edited

//     // MEMBERS
// private:
//     QMap<QString, QVector<Field>>   m_tables; // Map each field with it's used table
//     QSqlRecord                      *m_selectedRecord;
//     QVector<QSqlQuery>              m_updateQuerys;
//     int                             m_activeRecordIndex;
//     /*
//      * Used to set the behaviour of the model as an update model (that will display an existing record based on m-.recordIndex)
//      * or as an Insert model (which would display a newly generated empty record for you to insert)
//     */
//     BehaviourFlag                   m_behaviourFlag;
// };

// #endif // CRECMODEL_H
