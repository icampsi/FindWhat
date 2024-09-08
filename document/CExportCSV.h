/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QStandardItemModel>
#include <QString>
#include "CParsedFile.h"
#ifdef ENABLE_DBMANAGER
#include "dbManager/WSqlMultiTable.h"
#endif

class InvalidFileName_dlg;
class ProgBar_dlg;
class CPdfDoc;
class CPagedText;

class CExportCSV {
public:
    // CONSTRUCTORS AND DESTRUCTORS
    explicit CExportCSV();
    explicit CExportCSV(std::ifstream& in) { CExportCSV::deserialize(in); }

    ~CExportCSV() = default;

protected:
    // MEMBERS
    CEsquema    *m_associatedEsquema; // Esquema to use
    QString      m_exportFileRename;     // A string with placeholders for renaming files
    bool         m_renameParsedPDFFlag;  // A flag to state whether a pdf document should be renamed
    QString      m_fileNamePlaceholder;  // { "<data1>_<data2> some_fixed_text <data3> etc." } Use <dataName> for insert value dataName. etc.
    QString      m_idText;               // Text that id's a document so we only act on it if the string is found.
    // std::unordered_map<QString, QString> m_extractedValues;
    QVector<CParsedFile> m_files;       // Files associated with the document

    InvalidFileName_dlg *m_invalidFileNameDlg;   // Dialog that pops up if the resulting new file name contains invalid characters

    QStandardItemModel m_csvTableModel; // Table model for the csv format table
#ifdef ENABLE_DBMANAGER
    CSqlMultiTableModel m_dbTableModel;  // Table model for the db format table
#endif

public:
    // GETTERS AND SETTERS
    CEsquema* getAsocEsquema() const    { return m_associatedEsquema; }
    void setAsocEsquema(CEsquema* pEsq) { m_associatedEsquema = pEsq; }

    const QVector<CParsedFile>& getFiles() const { return m_files; }
    const QVector<CParsedFile>* setFiles(const std::vector<QString>& paths); // NEW BOOKMARK - CHANGED RETURN TYPE. WILL STICK?
    void deleteFile(int index) {
        m_files.removeAt(index);
        for(auto& file : m_files) {
            qDebug() << m_files.size();
            qDebug() << file.fileName();
        }
    }

    const QString& getExportFileRename() const        { return m_exportFileRename; }
    void setExportFileRename(const QString& fileName) { m_exportFileRename = fileName; }

    bool getRenameParsedPDFFlag() const             { return m_renameParsedPDFFlag; }
    void setRenameParsedPDFFlag(bool isToBeRenamed) { m_renameParsedPDFFlag = isToBeRenamed; }

    const QString& getFileNamePlaceholder() const           { return m_fileNamePlaceholder; }
    void setFileNamePlaceholder(const QString& placeholder) { m_fileNamePlaceholder = placeholder; }

    const QString& getIdText() const        { return m_idText; }
    void setIdText(const QString& idText)   { m_idText = idText; }

    QStandardItemModel *getCsvTableModel() { return &m_csvTableModel; }
#ifdef ENABLE_DBMANAGER
    CSqlMultiTableModel *getDbTableModel() { return &m_dbTableModel; }
#endif

    // PUBLIC FUNCTIONS
    // Functions to add and delete PDF file paths
    void addFileFromPath(const QString& path) { m_files.push_back(CParsedFile(path, &m_associatedEsquema)); }
    void deletePdfFile(int index); // BOOKMARK - Need implementation

    // Function to rename in pdf file
    void renameFile(const QString &oldFilePath);

    // Convert QStandardItemModel to std::vector<std::vector<QString>>
    void convertModelToVector(QAbstractItemModel* model, std::vector<std::vector<QString>>* format);

    // Parse values for all files stored in m_files
    bool parseFileValues(int index);
    // Extract values from the pdf's. Returns false if text couldn't be extracted.
    void buildStructure(QStandardItemModel *combinedModel, ProgBar_dlg* progressDialog, size_t maxColumns, bool dbParser = false);

    // SERIALIZATION
    void serialize(std::ofstream &out) const;
    void deserialize(std::ifstream &in);
};

#endif // CEXPORTCSV_H
