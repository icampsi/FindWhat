#include "dbManager/CDbConnection.h"
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>

bool CDbConnection::isInitialized = false;

// Method to get the singleton instance
CDbConnection& CDbConnection::getConnection(const QString &host, const QString &db, const QString &user, const QString &pass) {
    static CDbConnection instance(host, db, user, pass);

    if (!isInitialized && !host.isEmpty() && !db.isEmpty() && !user.isEmpty() && !pass.isEmpty()) {
        instance.connectDatabase(host, db, user, pass);
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

    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool CDbConnection::connectDatabase(const QString& host, const QString& db, const QString& user, const QString& pass) {
    // Set-up connection
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(host);
    m_db.setDatabaseName(db);
    m_db.setUserName(user);
    m_db.setPassword(pass);

    // Open connection
    if (m_db.open()) {
        qDebug() << "Connected";

        // Set-up query models after successful connection
        addModel("Members");
        addModel("Flats_view", "Flats");
        addModel("ru_bills", "Utility Bills");
        addModel("occupancy_view", "Occupancy");

        return true;
    } else {
        qDebug() << "Couldn't connect";
        return false;
    }
}

bool CDbConnection::addModel(const QString& tableName, const QString& alias) {
    const QString upAlias = alias.isEmpty() ? tableName : alias; // if alias was not provided, we use tableName as alias

    QSqlTableModel* model = new QSqlTableModel();
    model->setTable(tableName);
    bool ok = model->select();

    if (ok) {
        m_models.emplace(upAlias, model);
    } else {
        delete model;
    }
    return ok;
}

bool CDbConnection::bulkInsert(const QString& tableName, const std::vector<std::vector<QString>>& data) {
    if (!m_db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    if (data.empty() || data[0].empty()) {
        qWarning() << "Data is empty!";
        return false;
    }

    QSqlQuery query(m_db);

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
    if (!m_db.transaction()) {
        qWarning() << "Failed to start transaction:" << m_db.lastError();
        return false;
    }

    for (const auto& row : data) {
        // Ensure each row has the same number of columns
        if (row.size() != data[0].size()) {
            qWarning() << "Row size mismatch!";
            m_db.rollback();
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
            m_db.rollback();
            return false;
        }
    }

    // Commit transaction
    if (!m_db.commit()) {
        qWarning() << "Failed to commit transaction:" << m_db.lastError();
        return false;
    }

    if (m_models.find(tableName) != m_models.end()) {
        m_models.at(tableName)->select();
    }

    return true;
}
