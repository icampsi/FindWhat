/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QString>

#include "ui/dialogs/ProgBarExport_dlg.h"
#include "ui/dialogs/InvalidFileName_dlg.h"

class CEsquemaDoc;

class CExportCSV {
public:
    // CONSTRUCTORS AND DESTRUCTORS
    CExportCSV() : m_pdfFilePaths(), m_associatedEsquemaDoc(nullptr), m_csvFormat(), m_exportFileRename(), m_renameParsedPDFFlag(false), m_fileNamePlaceholder() {}
    ~CExportCSV() = default;

protected:
    // MEMBERS
    std::vector<QString> m_pdfFilePaths;         // File Paths associated with the document

    CEsquemaDoc*         m_associatedEsquemaDoc; // Esquema to use
    QString              m_csvFormat;            // A string made of csv each having the name of a formula or static data. this data will be replaced with the extracted data
    QString              m_exportFileRename;     // A string with placeholders for renaming files
    bool                 m_renameParsedPDFFlag;  // A flag to state whether a pdf document should be renamed
    QString              m_fileNamePlaceholder;  // { "<data1>_<data2> some_fixed_text <data3> etc." } Use <dataName> for insert value dataName. etc.

    InvalidFileName_dlg *m_invalidFileNameDlg;

public:
    // GETTERS AND SETTERS
    CEsquemaDoc* getAsocEsquemaDoc() const    { return m_associatedEsquemaDoc; }
    void setAsocEsquemaDoc(CEsquemaDoc* pDoc) { m_associatedEsquemaDoc = pDoc; }

    const QString& getCSVFormat() const      { return m_csvFormat; }
    void setCSVFormat(const QString& string) { m_csvFormat = string; }

    std::vector<QString> getPathFiles() const { return m_pdfFilePaths; }
    void setPathFiles(const std::vector<QString>& paths) {
        m_pdfFilePaths.clear();
        m_pdfFilePaths = paths;
    }

    const QString& getExportFileRename() const        { return m_exportFileRename; }
    void setExportFileRename(const QString& fileName) { m_exportFileRename = fileName; }

    bool getRenameParsedPDFFlag() const             { return m_renameParsedPDFFlag; }
    void setRenameParsedPDFFlag(bool isToBeRenamed) { m_renameParsedPDFFlag = isToBeRenamed; }

    const QString& getFileNamePlaceholder() const { return m_fileNamePlaceholder; }
    void setFileNamePlaceholder(const QString& fileNamePlaceholder) { m_fileNamePlaceholder = fileNamePlaceholder; }

    // PUBLIC FUNCTIONS
    // Functions to add and delete PDF file paths
    void addPdfFile(const QString& path)             { m_pdfFilePaths.push_back(std::move(path)); }
    const std::vector<QString>& getFilePaths() const { return m_pdfFilePaths; }
    void deletePdfFile(int index);

    // Function to reorder files inside the vector
    void reOrderFiles(size_t fileToMoveIndex, size_t targetPositionIndex);

    // Function to build XSV structure
    void buildXSVStructure(std::vector<std::vector<QString>> *xsvStructure, ProgBarExport_dlg *progressDialog = nullptr);

    // Function to rename in pdf file
    void renameFile(const QString &oldFilePath);

};

#endif // CEXPORTCSV_H
