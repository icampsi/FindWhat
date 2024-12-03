/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "dbManager/CDbConnection.h"
#include "CSqlMultiTableModel.h"

#include <QSqlError>
#include <QSqlQuery>

#include "sql_queries.h"

bool CDbConnection::isInitialized = false;

CDbConnection& CDbConnection::getConnection(const QString &host, const QString &db, const QString &user, const QString &pass) {
    static CDbConnection instance(host, db, user, pass);

    if (!isInitialized && !host.isEmpty() && !db.isEmpty() && !user.isEmpty() && !pass.isEmpty()) {
        instance.connectDatabase(host, db, user, pass);
        instance.addModel("Members", "Members"); // Table name, alias
        instance.addModel("Flats", "Flats");
        instance.addModel("ru_bills", "Utility Bills");
        instance.addModel("occupancy", "Occupancy");

        isInitialized = true;
    }

    return instance;
}

CDbConnection::CDbConnection(const QString& host, const QString& db, const QString& user, const QString& pass)
    : m_host(host), m_dbName(db), m_user(user), m_pass(pass) {}

CDbConnection::~CDbConnection() {
    for (auto& pair : m_models) {
        delete pair.second;
        pair.second = nullptr;
    }

    QSqlDatabase db = QSqlDatabase::database(m_dbName);
    if (db.isOpen()) db.close();
}

bool CDbConnection::connectDatabase(const QString& host, const QString& dbName, const QString& user, const QString& pass) {
    // Set-up connection
    QSqlDatabase::addDatabase("QMYSQL", m_dbName);
    qDebug() << m_dbName;
    QSqlDatabase db = QSqlDatabase::database(m_dbName);
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(pass);

    return db.open();
}

CSqlMultiTableModel *CDbConnection::addModel(const QString& tableName, const QString& alias) {
    const QString upAlias = alias.isEmpty() ? tableName : alias; // if alias was not provided, we use tableName as alias

    CSqlMultiTableModel* model = new CSqlMultiTableModel(this);
    QSqlDatabase db = QSqlDatabase::database(m_dbName);

    const QString selQuery = retrieveSelQuery(tableName);

    model->setQuery(selQuery, db);

    m_models.emplace(upAlias, model);
    return model;
}

void CDbConnection::deleteModel(std::unordered_map<QString, CSqlMultiTableModel*>& models, const QString& key) {
    auto it = models.find(key);
    if (it != models.end()) {
        // Delete model
        delete it->second;
        // Remove entry from map
        models.erase(it);
    }
}

bool CDbConnection::bulkInsert(const QString& tableName, const std::vector<std::vector<QString>>& data) {
    QSqlDatabase db = QSqlDatabase::database(m_dbName);
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    if (data.empty() || data[0].empty()) {
        qWarning() << "Data is empty!";
        return false;
    }

    QSqlQuery query(db);

    // Constructing the SQL insert statement with placeholders for each value
    QString insertStr = QString("INSERT INTO %1 VALUES (").arg(tableName);
    insertStr.append(QString("?, ").repeated(data[0].size()));
    insertStr.chop(2); // Remove last comma and space
    insertStr.append(")");

    // Prepare the query with the constructed SQL statement
    if (!query.prepare(insertStr)) {
        qWarning() << "Failed to prepare query:" << query.lastError().text();
        return false;
    }

    // Log the prepared statement
    qDebug() << "Prepared insert query:" << insertStr;

    // Begin transaction
    if (!db.transaction()) {
        qWarning() << "Failed to start transaction:" << db.lastError();
        return false;
    }

    for (const auto& row : data) {
        // Ensure each row has the same number of columns
        if (row.size() != data[0].size()) {
            qWarning() << "Row size mismatch!";
            db.rollback();
            return false;
        }

        // Bind values to the prepared statement
        for (const auto& value : row) {
            query.addBindValue(value);
        }

        // Log the bound values
        qDebug() << "Bound values:" << query.boundValues();

        // Execute the query
        if (!query.exec()) {
            qWarning() << "Insert failed:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Commit transaction
    if (!db.commit()) {
        qWarning() << "Failed to commit transaction:" << db.lastError();
        return false;
    }

    return true;
}

const QString CDbConnection::retrieveSelQuery(const QString& tableName) const {
    return  (tableName == "Members")   ? SqlQueries::GetMembers   :
            (tableName == "Flats")     ? SqlQueries::GetFlats     :
            (tableName == "ru_bills")  ? SqlQueries::GetRuBills   :
            (tableName == "occupancy") ? SqlQueries::GetOccupancy :
            throw std::invalid_argument("CDbConnection::addModel -> Invalid table name");
}

void CDbConnection::refreshModel(const QString& tableName) {
    CSqlMultiTableModel *model = getModel(tableName);
    QSqlDatabase db = QSqlDatabase::database(m_dbName);
    model->setQuery(model->lastQuery(), db);
}
