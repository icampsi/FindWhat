/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CSqlMultiTableModel.h"

#include <QSqlError>
#include <QSqlIndex>
#include <QSqlField>
#include <QSqlRecord>

CSqlMultiTableModel::CSqlMultiTableModel(QObject *parent, int index, QString query, QSqlDatabase db)
    : QAbstractTableModel(parent), m_activeRecordIndex(index), m_rowCount{ 0 }
{
    if(!query.isEmpty()) {
        setQuery(query, db);
    }
}

int CSqlMultiTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_rowCount;
}

int CSqlMultiTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_query.record().count();
}


QString CSqlMultiTableModel::processFieldName(const QString& fieldName) const {
    QString result;
    result.reserve(fieldName.size() + fieldName.count('_'));

    bool isNewWord = true;
    for (const QChar& currentChar : fieldName) {
        if (currentChar == '_') {
            result += ' ';  // Replace underscore with space
            isNewWord = true;  // Indicate that the next character is the start of a new word
        } else if (currentChar.isUpper() && !result.isEmpty() && !isNewWord) {
            result += ' ';  // Add a space before capital letters that are not at the start of a new word
            result += currentChar;
            isNewWord = false;  // Continue as part of the current word
        } else {
            result += (isNewWord && currentChar.isLower()) ? currentChar.toUpper() : currentChar;
            isNewWord = false;  // Reset after processing the current character
        }
    }

    return result;
}


QVariant CSqlMultiTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    int recIndex = index.row();

    // Retrieve the record for the current field (column) and attribute/row
    const int col = index.column();

    QString fieldName = m_records.at(recIndex).fieldName(col);
    QString tableName = m_records.at(recIndex).field(col).tableName();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_records.at(recIndex).value(col);
    } else if (role == Qt::UserRole + 1) {
        // Custom role: Check if there's foreign key information for the field
        const QVector<Field>& fields = m_tables.value(tableName);
        for (const Field& field : fields) {
            if (field.m_fieldName == fieldName) {
                if (field.m_hasFk) {
                    // Return foreign key values as a QVariantList
                    return QVariant::fromValue(field.m_fkColumnValues);
                }
                break;
            }
        }
    }

    return QVariant();
}

QVariant CSqlMultiTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(orientation); Q_UNUSED(role);
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return processFieldName(m_query.record().fieldName(section));
    }
    else return QVariant();
}

bool CSqlMultiTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    // Get the row and column indices
    const int col = index.column();
    const int row = index.row();

    if (col >= 0 && col < m_query.record().count()) {
        // Update the record with the new value
        m_records[row].setValue(col, value);

        // Notify that the data has changed
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        emit cellValueChanged(col);
        return true;
    }

    return false;
}

Qt::ItemFlags CSqlMultiTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    return (editableCheck(index)) ? (defaultFlags & Qt::ItemIsEditable) : (defaultFlags | Qt::ItemIsEditable); // Value editable
}

QVariant CSqlMultiTableModel::fieldName(int index) const {
    const int recCount = m_query.record().count();
    if (index < 0 || index >= recCount) {
        return QVariant();
    }
    return m_query.record().fieldName(index); // Field names
}

void CSqlMultiTableModel::changeRecord(int index) {
    m_activeRecordIndex = index;
}

void CSqlMultiTableModel::setQuery(const QString &query, const QSqlDatabase &db) {
    if(query.contains("AS")) {
        qWarning() << "CSqlMultiTableModel::setQuery: Query shouldn't contain aliases";
        return;
    }

    // Clear data
    m_records.clear();
    // Build and execute query
    m_query = QSqlQuery(db);
    m_query.prepare(query);
    m_query.exec();

    // Copy query data into local vector
    m_records.reserve(m_query.size());
    while(m_query.next()) {
        m_records.append(m_query.record());
    }

    extractTables();
    formUpsertQuery();

    // Assaign table size
    int qSize = m_query.size();
    qSize >= 0 ? m_rowCount = qSize : m_rowCount = 0;
}

void CSqlMultiTableModel::setQuery(QSqlQuery &&query) {
    // Block aliases in queries
    if(query.lastQuery().contains("AS")) {
        qWarning() << "CSqlMultiTableModel::setQuery: Query shouldn't contain aliases";
        return;
    }

    // Clear data
    m_records.clear();
    // Build and execute query
    m_query = std::move(query);
    m_query.exec();

    // Copy query data into local vector
    m_records.reserve(m_query.size());
    while(m_query.next()) {
        m_records.append(m_query.record());
    }

    extractTables();
    formUpsertQuery();

    // Assaign table size
    int qSize = m_query.size();
    qSize >= 0 ? m_rowCount = qSize : m_rowCount = 0;
}

CSqlMultiTableModel::Field CSqlMultiTableModel::retrieveForeignKeyInfo(const QString& tableName, const QString& columnName) {
    QSqlDatabase db = QSqlDatabase::database("closca");
    Field field(columnName);

    // Query to retrieve foreign key information for the specified table and column
    QString queryInfo = QString(
                            "SELECT COLUMN_NAME, "
                            "REFERENCED_TABLE_NAME, "
                            "REFERENCED_COLUMN_NAME "
                            "FROM information_schema.KEY_COLUMN_USAGE "
                            "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = '%1' "
                            "AND COLUMN_NAME = '%2' "
                            "AND REFERENCED_TABLE_NAME IS NOT NULL"
                            ).arg(tableName, columnName);

    QSqlQuery query(db);
    if (query.exec(queryInfo)) {
        if (query.next()) {
            QString referencedTable  = query.value("REFERENCED_TABLE_NAME").toString();
            QString referencedColumn = query.value("REFERENCED_COLUMN_NAME").toString();
            field.m_hasFk = true;
            field.m_fkTableName  = referencedTable;
            field.m_fkColumnName = referencedColumn;

            // fetch and store values from the referenced column
            QSqlQuery queryValues(QString("SELECT %1 FROM %2").arg(referencedColumn, referencedTable), db);
            if (queryValues.exec()) {
                while (queryValues.next()) {
                    QString pkValue = queryValues.value(0).toString();
                    field.m_fkColumnValues.append(pkValue);
                }
            } else {
                qWarning() << "Query for retreaving pk values failed:" << queryValues.lastError().text();
            }
        }
    } else {
        qWarning() << "Error executing query:" << query.lastError().text();
    }
    return field;
}

void CSqlMultiTableModel::extractTables() {
    m_tables.clear();
    for (int i = 0; i < m_query.record().count(); ++i) {
        QString tableName = m_query.record().field(i).tableName();
        if (!tableName.isEmpty()) { // Check if the tableName is valid (non-empty)
            const Field field = retrieveForeignKeyInfo(tableName,m_query.record().field(i).name());
            if (m_tables.contains(tableName)) {
                // If the table already exists in the map, append the field to the vector
                m_tables[tableName].append(field);
            } else {
                // If the table doesn't exist, create a new vector and add the field
                m_tables.insert(tableName, QVector<Field>() << field);
            }
        }
    }
}

void CSqlMultiTableModel::formUpsertQuery() {
    m_updateQuerys.clear();

    for (const QString& tableName : m_tables.keys()) {
        QStringList fieldNames;
        QStringList placeholders;
        QStringList updateClauses;
        QVariantList values;

        // Construct the field names, placeholders, and update clauses
        for (const Field& field : m_tables.value(tableName)) {
            const QString& fieldName = field.m_fieldName;
            fieldNames.append(fieldName);
            placeholders.append(QString(":%1").arg(fieldName));
            if (fieldName != QSqlDatabase::database("closca").primaryIndex(tableName).fieldName(0)) {
                updateClauses.append(QString("%1 = VALUES(%1)").arg(fieldName));
            }
            values.append(m_query.record().value(fieldName));
        }

        // Join the lists into a single string
        QString fieldNamesStr = fieldNames.join(", ");
        QString placeholdersStr = placeholders.join(", ");
        QString updateClauseStr = updateClauses.join(", ");

        // Construct the final INSERT ... ON DUPLICATE KEY UPDATE query string
        QString upsertQuery = QString("INSERT INTO %1 (%2) VALUES (%3) ON DUPLICATE KEY UPDATE %4")
                                  .arg(tableName,
                                  fieldNamesStr,
                                  placeholdersStr,
                                  updateClauseStr);
        // Prepare and execute the query
        QSqlQuery upsertQry(QSqlDatabase::database("closca"));
        upsertQry.prepare(upsertQuery);

        m_updateQuerys.append(std::move(upsertQry));
    }
}

bool CSqlMultiTableModel::commitRecord(const int row) {
    QSqlDatabase db = QSqlDatabase::database("closca");

    // Start a transaction
    if (!db.transaction()) {
        qWarning() << "Failed to start database transaction:" << db.lastError().text();
        return false;
    }

    // Iterate through all update queries for the row
    for (int i = 0; i < m_updateQuerys.size(); ++i) {
        QSqlQuery& query = m_updateQuerys[i];
        const QString tableName = m_tables.keys().at(i);  // Get the table name from the map

        // Bind the values to the query
        for (const Field& field : m_tables.value(tableName)) {
            const QString& fieldName = field.m_fieldName;
            query.bindValue(QString(":%1").arg(fieldName), m_records.at(row).value(fieldName));
        }

        // Execute the pre-bound query
        if (!query.exec()) {
            qWarning() << "Failed to execute update query for table" << tableName << ":" << query.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Commit the transaction if all queries succeeded
    if (!db.commit()) {
        qWarning() << "Failed to commit database transaction:" << db.lastError().text();
        return false;
    }

    return true;
}

bool CSqlMultiTableModel::commitTable() {
    QSqlDatabase db = QSqlDatabase::database("closca");

    // Start a transaction
    if (!db.transaction()) {
        qWarning() << "Failed to start database transaction:" << db.lastError().text();
        return false;
    }

    for(int recIndex{0}; recIndex < m_rowCount; ++recIndex) {
        // Iterate through all update queries for each row
        for (int i = 0; i < m_updateQuerys.size(); ++i) {
            QSqlQuery& query = m_updateQuerys[i];
            const QString tableName = m_tables.keys().at(i);  // Get the table name from the map

            // Bind the values to the query
            for (const Field& field : m_tables.value(tableName)) {
                const QString& fieldName = field.m_fieldName;
                query.bindValue(QString(":%1").arg(fieldName), m_records.at(recIndex).value(fieldName));
            }

            // Execute the pre-bound query
            if (!query.exec()) {
                qWarning() << "\nFailed to execute update query:\n" << m_updateQuerys[i].executedQuery() << "\nFor table" << tableName << "\n Error is:\n" << query.lastError().text();
                db.rollback();
                return false;
            }
        }
    }

    // Commit the transaction if all queries succeeded
    if (!db.commit()) {
        qWarning() << "Failed to commit database transaction:" << db.lastError().text();
        return false;
    }

    return true;
}

bool CSqlMultiTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    if(!(count > 0)) return false;
    m_rowCount += count;
    beginInsertRows(parent, row, row + count - 1);
    for(int i{0}; i < count; i++) {
        m_records.append(QSqlRecord(m_query.record()));
    }
    endInsertRows();
    return true;
}

bool CSqlMultiTableModel::editableCheck(const QModelIndex &index) const {
    // tableName being empty means that we can't reference it on the query
    return (m_query.record().field(index.row()).tableName().isEmpty());
}
