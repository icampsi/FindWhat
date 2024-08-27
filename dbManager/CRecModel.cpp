/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CRecModel.h"

#include <QSqlError>
#include <QSqlIndex>
#include <QSqlTableModel>
#include <QSqlField>
#include <QSqlRecord>

CRecModel::CRecModel(QObject *parent, int index, QString query, QSqlDatabase db)
    : QSqlQueryModel(parent), m_selectedRecord{nullptr}, m_activeRecordIndex(index), m_behaviourFlag { BehaviourFlag::Update }
{
    if(!query.isEmpty()) {
        setQuery(query, db);
    }
}

CRecModel::~CRecModel() {}

int CRecModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return record().count(); // One row for each field
}

int CRecModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2; // "Field" and "Value"
}

QString CRecModel::processFieldName(const QString& fieldName) const {
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


QVariant CRecModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    // Retrieve the record for the current field (row) and attribute/column
    const int col = index.column(); // Attribute or other aspect
    const int row = index.row();    // Field corresponding to the row
    const int recCount = m_selectedRecord->count();

    if (row >= 0 && row < recCount) {
        QString fieldName = m_selectedRecord->fieldName(row); // Now row corresponds to field
        QString tableName = m_selectedRecord->field(row).tableName();

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            // Display the field name in the first column, or the value in other columns
            return (col == 0) ? processFieldName(fieldName) : m_selectedRecord->value(row);
        } else if (role == Qt::UserRole + 1 && col == 1) {
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
        } else {
            return QSqlQueryModel::data(index, role);
        }
    }

    return QVariant();
}


QVariant CRecModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return "Field";
        else if (section == 1)
            return "Value";
    }
    return QVariant();
}

bool CRecModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole || index.column() != 1) {
        return false;
    }

    // Get the row and column indices
    const int row = index.row();

    // Obtain the field name from the record
    const QString fieldName = m_selectedRecord->field(row).name();

    if (row >= 0 && row < m_selectedRecord->count()) {
        // Update the record with the new value
        m_selectedRecord->setValue(fieldName, value);

        // Notify that the data has changed
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        emit cellValueChanged(row);
        return true;
    }

    return false;
}

Qt::ItemFlags CRecModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    if (index.column() == 0) {
        return defaultFlags & ~Qt::ItemIsEditable; // Field non-editable
    }

    return (editableCheck(index)) ? (defaultFlags & Qt::ItemIsEditable) : (defaultFlags | Qt::ItemIsEditable); // Value editable
}

QVariant CRecModel::value(int index, int role) const {
    const int recCount = m_selectedRecord->count();

    if (index < 0 || index >= recCount || role != Qt::DisplayRole) {
        return QVariant();
    }
    return m_selectedRecord->value(index); // Field values
}

QVariant CRecModel::fieldName(int index, int role) const {
    const int recCount = m_selectedRecord->count();
    if (index < 0 || index >= recCount || role != Qt::DisplayRole) {
        return QVariant();
    }
    return m_selectedRecord->fieldName(index); // Field names
}

void CRecModel::changeRecord(int index) {
    m_activeRecordIndex = index;
    retrieveRecord();
}

void CRecModel::retrieveRecord() {
    if (m_behaviourFlag == BehaviourFlag::Insert) {
        // Create an empty record based on the table schema
        QSqlRecord emptyRecord = query().record();
        for (int i = 0; i < emptyRecord.count(); ++i) {
            emptyRecord.setNull(i);  // Set all fields to NULL
        }
        m_selectedRecord = new QSqlRecord(emptyRecord);
    } else {
        const_cast<QSqlQuery&>(query()).seek(m_activeRecordIndex);
        m_selectedRecord = new QSqlRecord(query().record());
    }
}

void CRecModel::setQuery(const QString &query, const QSqlDatabase &db) {
    if(query.contains("AS")) {
        qWarning() << "CRecModel::setQuery: Query shouldn't contain aliases";
        return;
    }

    QSqlQueryModel::setQuery(query, db);
    retrieveRecord();
    extractTables();
    formUpsertQuery();
}

void CRecModel::setQuery(QSqlQuery &&query) {
    // Block aliases in queries
    if(query.lastQuery().contains("AS")) {
        qWarning() << "CRecModel::setQuery: Query shouldn't contain aliases";
        return;
    }

    QSqlQueryModel::setQuery(std::move(query));
    retrieveRecord();
    extractTables();
    formUpsertQuery();
}


CRecModel::Field CRecModel::retrieveForeignKeyInfo(const QString& tableName, const QString& columnName) {
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
                            ).arg(tableName).arg(columnName);

    QSqlQuery query(db);
    if (query.exec(queryInfo)) {
        if (query.next()) {
            QString referencedTable  = query.value("REFERENCED_TABLE_NAME").toString();
            QString referencedColumn = query.value("REFERENCED_COLUMN_NAME").toString();
            field.m_hasFk = true;
            field.m_fkTableName  = referencedTable;
            field.m_fkColumnName = referencedColumn;

            // fetch and store values from the referenced column
            QSqlQuery queryValues(QString("SELECT %1 FROM %2").arg(referencedColumn).arg(referencedTable), db);
            if (queryValues.exec()) {
                while (queryValues.next()) {
                    QString pkValue = queryValues.value(0).toString();
                    field.m_fkColumnValues.append(pkValue);
                }
            } else {
                qDebug() << "Query for retreaving pk values failed:" << queryValues.lastError().text();
            }
        }
    } else {
        qDebug() << "Error executing query:" << query.lastError().text();
    }
    return field;
}

void CRecModel::extractTables() {
    for (int i = 0; i < m_selectedRecord->count(); ++i) {
        QString tableName = m_selectedRecord->field(i).tableName();
        if (!tableName.isEmpty()) { // Check if the tableName is valid (non-empty)
            const Field field = retrieveForeignKeyInfo(tableName, m_selectedRecord->field(i).name());
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

void CRecModel::formUpsertQuery() {
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
            values.append(m_selectedRecord->value(fieldName));
        }

        // Join the lists into a single string
        QString fieldNamesStr = fieldNames.join(", ");
        QString placeholdersStr = placeholders.join(", ");
        QString updateClauseStr = updateClauses.join(", ");

        // Construct the final INSERT ... ON DUPLICATE KEY UPDATE query string
        QString upsertQuery = QString("INSERT INTO %1 (%2) VALUES (%3) ON DUPLICATE KEY UPDATE %4")
                                  .arg(tableName)
                                  .arg(fieldNamesStr)
                                  .arg(placeholdersStr)
                                  .arg(updateClauseStr);
        // Prepare and execute the query
        QSqlQuery upsertQry(QSqlDatabase::database("closca"));
        upsertQry.prepare(upsertQuery);

        // Bind the values to the query
        for (const Field& field : m_tables.value(tableName)) {
            const QString& fieldName = field.m_fieldName;
            upsertQry.bindValue(QString(":%1").arg(fieldName), m_selectedRecord->value(fieldName));
        }

        m_updateQuerys.append(std::move(upsertQry));
    }
}

bool CRecModel::submitRecord() {
    QSqlDatabase db = QSqlDatabase::database("closca");

    // Start a transaction
    if (!db.transaction()) {
        qWarning() << "Failed to start database transaction:" << db.lastError().text();
        return false;
    }

    // Iterate through all update queries
    for (int i = 0; i < m_updateQuerys.size(); ++i) {
        QSqlQuery& query = m_updateQuerys[i];
        const QString tableName = m_tables.keys().at(i);  // Get the table name from the map
        const QVector<Field>& fields = m_tables.value(tableName);  // Get the fields for this table

        // Get the primary key field and value
        QString pkName = db.primaryIndex(tableName).fieldName(0);  // BOOKMARK - Only works if primary key is the first field
        QVariant pkValue = m_selectedRecord->value(pkName);

        if (pkValue.isNull()) {
            qWarning() << "Primary key value is null for table" << tableName;
            db.rollback();
            return false;
        }

        // Bind the field values to the query
        for (const Field& field : fields) {
            const QString& fieldName = field.m_fieldName;
            query.bindValue(QString(":").append(fieldName), m_selectedRecord->value(fieldName));
        }
        // Bind the primary key value
        query.bindValue(":pkV", pkValue);

        // Execute the query
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

bool CRecModel::editableCheck(const QModelIndex &index) const {
    // tableName being empty means that we can't reference it on the query
    return (m_selectedRecord->field(index.row()).tableName().isEmpty());
}
