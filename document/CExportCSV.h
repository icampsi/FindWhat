/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QStandardItemModel>
#include <QString>

#ifdef ENABLE_DBMANAGER
class CRecModel;
#include "dbManager/WRecEditTable.h"
#endif

class InvalidFileName_dlg;
class ProgBarExport_dlg;
class CEsquemaDoc;
class CPdfDoc;
class CPagedText;
class CExportCSV {

public:
    // CONSTRUCTORS AND DESTRUCTORS
    explicit CExportCSV()
        : m_pdfFilePaths(),
        m_associatedEsquemaDoc(nullptr),
        m_exportFileRename(),
        m_renameParsedPDFFlag(false),
        m_fileNamePlaceholder(),
        m_idText(""),
        m_csvTableModel(10, 10)
#ifdef ENABLE_DBMANAGER
        , m_dbTableModel()  // Table model for the db format table
#endif
    {}
    explicit CExportCSV(std::ifstream& in) { CExportCSV::deserialize(in); }

    ~CExportCSV() = default;

protected:
    // MEMBERS    
    std::vector<QString> m_pdfFilePaths; // File Paths associated with the document

    CEsquemaDoc *m_associatedEsquemaDoc; // Esquema to use
    QString      m_exportFileRename;     // A string with placeholders for renaming files
    bool         m_renameParsedPDFFlag;  // A flag to state whether a pdf document should be renamed
    QString      m_fileNamePlaceholder;  // { "<data1>_<data2> some_fixed_text <data3> etc." } Use <dataName> for insert value dataName. etc.
    QString      m_idText;               // Text that id's a document so we only act on it if the string is found.

    InvalidFileName_dlg *m_invalidFileNameDlg;   // Dialog that pops up if the resulting new file name contains invalid characters

    QStandardItemModel m_csvTableModel; // Table model for the csv format table
#ifdef ENABLE_DBMANAGER
    CRecModel m_dbTableModel;  // Table model for the db format table
#endif

public:
    // GETTERS AND SETTERS
    CEsquemaDoc* getAsocEsquemaDoc() const    { return m_associatedEsquemaDoc; }
    void setAsocEsquemaDoc(CEsquemaDoc* pDoc) { m_associatedEsquemaDoc = pDoc; }

    const std::vector<QString>& getPathFiles() const { return m_pdfFilePaths; }
    void setPathFiles(const std::vector<QString>& paths) {
        m_pdfFilePaths.clear();
        m_pdfFilePaths = paths;
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
    CRecModel *getDbTableModel() { return &m_dbTableModel; }
#endif

    // PUBLIC FUNCTIONS
    // Functions to add and delete PDF file paths
    void addPdfFile(const QString& path)             { m_pdfFilePaths.push_back(std::move(path)); }
    const std::vector<QString>& getFilePaths() const { return m_pdfFilePaths; }
    void deletePdfFile(int index); // BOOKMARK - Need implementation

    // Function to rename in pdf file
    void renameFile(const QString &oldFilePath);

    // Convert QStandardItemModel to std::vector<std::vector<QString>>
    void convertModelToVector(QAbstractItemModel* model, std::vector<std::vector<QString>>* format);

    void buildStructure(QStandardItemModel* combinedModel, ProgBarExport_dlg* progressDialog, size_t maxColumns, bool dbParser = false);

    // SERIALIZATION
    void serialize(std::ofstream &out) const;
    void deserialize(std::ifstream &in);
};

#endif // CEXPORTCSV_H
