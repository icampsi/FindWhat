/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include "qstandarditemmodel.h"
#include <QString>

class InvalidFileName_dlg;
class ProgBarExport_dlg;
class CEsquemaDoc;

class CExportCSV {

public:
    // CONSTRUCTORS AND DESTRUCTORS
    explicit CExportCSV() : m_pdfFilePaths(), m_associatedEsquemaDoc(nullptr), m_exportFileRename(), m_renameParsedPDFFlag(false), m_fileNamePlaceholder(), m_idText(""), m_tableModel(10, 10) {}
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

    QStandardItemModel m_tableModel;

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

    // PUBLIC FUNCTIONS
    // Functions to add and delete PDF file paths
    void addPdfFile(const QString& path)             { m_pdfFilePaths.push_back(std::move(path)); }
    const std::vector<QString>& getFilePaths() const { return m_pdfFilePaths; }
    void deletePdfFile(int index); // BOOKMARK - Need implementation

    // Function to reorder files inside the vector
    void reOrderFiles(size_t fileToMoveIndex, size_t targetPositionIndex);

    // Function to rename in pdf file
    void renameFile(const QString &oldFilePath);

    // Convert QStandardItemModel to std::vector<std::vector<QString>>
    std::vector<std::vector<QString>> convertModelToVector(QStandardItemModel* model);

    void buildStructure(std::vector<std::vector<QString>> *xsvStructure, ProgBarExport_dlg *progressDialog, size_t maxColumns);

    QStandardItemModel *getTableModel() { return &m_tableModel; }

    // SERIALIZATION
    void serialize(std::ofstream &out) const;
    void deserialize(std::ifstream &in);
};

#endif // CEXPORTCSV_H
