/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CDBCONNECTION_H
#define CDBCONNECTION_H

#include <QObject>

class CSqlMultiTableModel;

class CDbConnection : QObject {
    Q_OBJECT
public:
    static CDbConnection& getConnection(const QString &host = "", const QString &db = "", const QString &user = "", const QString &pass = "");

    // Delete copy constructor and assignment operator to ensure singleton properties
    CDbConnection(const CDbConnection&) = delete;
    CDbConnection& operator=(const CDbConnection&) = delete;

    ~CDbConnection();

private:
    explicit CDbConnection(const QString &host, const QString &db, const QString &user, const QString &pass);

public:
    // Creates a database connection
    bool connectDatabase(const QString& host, const QString& dbName, const QString& user, const QString& pass);

    const QString& host()       const { return m_host;   }
    const QString& dbName()     const { return m_dbName; }
    const QString& user()       const { return m_user;   }
    const QString& password()   const { return m_pass;   }

    // Returns the model pointing to the table with the requested alias
    CSqlMultiTableModel* getModel(const QString& tableAlias) { return m_models.at(tableAlias); }

    // This will add a new model pointing to a new db table. It doesent create the table or change anything from the db! table must already exist.
    // alias will be the given key to retrieve it from the map.
    CSqlMultiTableModel *addModel(const QString& tableName, const QString& alias = "");
    void deleteModel(std::unordered_map<QString, CSqlMultiTableModel*>& models, const QString& key);
    bool bulkInsert(const QString& tableName, const std::vector<std::vector<QString>>& data);

signals:
    void queryChanged();

private:
    std::unordered_map<QString, CSqlMultiTableModel*> m_models;
    QString m_host;
    QString m_dbName;
    QString m_user;
    QString m_pass;

    static bool isInitialized;
};

#endif // CDBCONNECTION_H
