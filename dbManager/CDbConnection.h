#ifndef CDBCONNECTION_H
#define CDBCONNECTION_H

#include <QSqlTableModel>
#include <QSqlDatabase>

class CDbConnection {
public:
    enum class TableName { Members, Flats, Utility_Bills, Occupancy };

    static CDbConnection& getConnection(const QString &host = "", const QString &db = "", const QString &user = "", const QString &pass = "");

    // Delete copy constructor and assignment operator to ensure singleton properties
    CDbConnection(const CDbConnection&) = delete;
    CDbConnection& operator=(const CDbConnection&) = delete;

    ~CDbConnection();

private:
    explicit CDbConnection(const QString &host, const QString &db, const QString &user, const QString &pass);

public:
    // Creates a database connection
    bool connectDatabase(const QString& host, const QString& db, const QString& user, const QString& pass);
    // Closes the database connection. The database is also closed in the destructor so this is only for explicit closeing.
    void closeDb() { m_db.close(); }
    // Returns the model pointing to the table with the requested name
    QSqlTableModel* getModel(const QString& tableName) { return m_models.at(tableName); }

    // This will add a new model pointing to a new table. It doesent create the table or change anything from the db! table must already exist.
    // alias will be the given key to retrieve it from the map.
    bool addModel(const QString& tableName, const QString& alias = "");
    bool bulkInsert(const QString& tableName, const std::vector<std::vector<QString>>& data);

signals:
    void queryChanged();

private:
    QSqlDatabase m_db;
    std::unordered_map<QString, QSqlTableModel*> m_models;
    QString m_host;
    QString m_dbName;
    QString m_user;
    QString m_pass;

    static bool isInitialized;
};

#endif // CDBCONNECTION_H
