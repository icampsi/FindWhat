/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "dbManager/CDbConnection.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>

bool CDbConnection::isInitialized = false;

CDbConnection& CDbConnection::getConnection(const QString &host, const QString &db, const QString &user, const QString &pass) {
    static CDbConnection instance(host, db, user, pass);

    if (!isInitialized && !host.isEmpty() && !db.isEmpty() && !user.isEmpty() && !pass.isEmpty()) {
        instance.connectDatabase(host, db, user, pass);
        instance.addModel("Members");
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

QSqlRelationalTableModel *CDbConnection::addModel(const QString& tableName, const QString& alias) {
    const QString upAlias = alias.isEmpty() ? tableName : alias; // if alias was not provided, we use tableName as alias

    QSqlRelationalTableModel* model = new QSqlRelationalTableModel(this, QSqlDatabase::database(m_dbName));
    model->setTable(tableName);
    //model->select();

    if (model) {
        m_models.emplace(upAlias, model);
    } else {
        delete model;
    }
    return model;
}

void CDbConnection::deleteModel(std::unordered_map<QString, QSqlRelationalTableModel*>& models, const QString& key) {
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

    if (m_models.find(tableName) != m_models.end()) {
        m_models.at(tableName)->select();
    }

    return true;
}
