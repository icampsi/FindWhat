#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QString>

#include "ui/dialogs/ExportCSVProgressBar_dlg.h"
#include "src/CData.h"
#include "ui/dialogs/InvalidFileName_dlg.h"

class CEsquemaDoc;

class CExportCSV {
public:
    // CONSTRUCTORS AND DESTRUCTORS
    CExportCSV() : m_pdfFilePaths(), m_associatedEsquemaDoc(nullptr), m_csvFormat(), m_exportFileRename(), m_renameParsedPDFFlag(false) {}
    ~CExportCSV() = default;

protected:
    // MEMBERS
    std::vector<QString> m_pdfFilePaths;         // File Paths associated with the document
    CEsquemaDoc*         m_associatedEsquemaDoc; // Esquema to use
    QString              m_csvFormat;            // A string made of csv each having the name of a formula or static data. this data will be replaced with the extracted data
    QString              m_exportFileRename;     // A string with placeholders for renaming files
    bool                 m_renameParsedPDFFlag;  // A flag to state whether a pdf document should be renamed

    InvalidFileName_dlg *m_invalidFileNameDlg;


public:
    // GETTERS AND SETTERS
    CEsquemaDoc* getAsocEsquemaDoc() const    { return m_associatedEsquemaDoc; }
    void setAsocEsquemaDoc(CEsquemaDoc* pDoc) { m_associatedEsquemaDoc = pDoc; }

    void setCSVFormat(const QString& string) { m_csvFormat = string; }
    const QString& getCSVFormat()            { return m_csvFormat; }

    std::vector<QString> getPathFiles() const { return m_pdfFilePaths; }
    void setPathFiles(const std::vector<QString>& paths) {
        m_pdfFilePaths.clear();
        m_pdfFilePaths = paths;
    }

    void setExportFileRename(const QString& fileName) { m_exportFileRename = fileName; }
    const QString& getExportFileRename() const        { return m_exportFileRename; }

    void setRenameParsedPDFFlag(bool isToBeRenamed) { m_renameParsedPDFFlag =  isToBeRenamed; }
    const bool getRenameParsedPDFFlag() const { return m_renameParsedPDFFlag; }

    // PUBLIC FUNCTIONS
    // Functions to add and delete PDF file paths
    void addPdfFile(const QString& path)             { m_pdfFilePaths.push_back(path); }
    const std::vector<QString>& getFilePaths() const { return m_pdfFilePaths; }
    void deletePdfFile(int index);

    // Function to reorder files inside the vector
    void reOrderFiles(int fileToMoveIndex, int targetPositionIndex);

    // Function to build XSV structure
    void buildXSVStructure(std::vector<std::vector<QString> > &xsvStructure, ExportCSVProgressBar_dlg *progressDialog = nullptr);

    // Function to rename in pdf file
    void renameFile(const QString &oldFilePath);

};

#endif // CEXPORTCSV_H
