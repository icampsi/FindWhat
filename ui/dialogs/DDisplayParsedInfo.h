/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef DDISPLAYPARSEDINFO_H
#define DDISPLAYPARSEDINFO_H

#include <QDialog>

class CParsedPdfModel;
class QSqlDatabase;

namespace Ui { class DDisplayParsedInfo; }

class DDisplayParsedInfo : public QDialog {
    Q_OBJECT

public:
    explicit DDisplayParsedInfo(CParsedPdfModel *combinedModel, bool isCSVParser, QWidget *parent = nullptr);
    ~DDisplayParsedInfo();
    // Exports the struct to a .csv
    bool exportToCSV();
#ifdef ENABLE_DBMANAGER
    bool commitTable();
#endif

private slots:
    void on_pushButton_save_clicked();

    //Protected functions
protected:
#ifdef ENABLE_DBMANAGER
    int getFlatID(QSqlDatabase &db, const QString &floor_num, const QString &door_num);
    int getUtilityID(QSqlDatabase &db, const QString &utility_name);
    void insertEntries(QSqlDatabase &db, CParsedPdfModel &model, const QStringList &pdfHashes);
#endif

    // Members
protected:
    Ui::DDisplayParsedInfo *ui;
    CParsedPdfModel *m_combinedModel;
    bool m_isCSVParser;
};

#endif // DDISPLAYPARSEDINFO_H
