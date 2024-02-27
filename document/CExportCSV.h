#ifndef CEXPORTCSV_H
#define CEXPORTCSV_H

#include <QString>
#include "ui/dialogs/ExportCSVProgressBar_dlg.h"
#include "src/CData.h"

class CEsquemaDoc;

class CExportCSV {
public:
    // Constructors and destructor
    CExportCSV() : m_pdfFilePaths(), m_associatedEsquemaDoc(nullptr) {}
    ~CExportCSV() = default;

protected:
    std::vector<QString> m_pdfFilePaths; // File Paths associated with the document
    CEsquemaDoc*         m_associatedEsquemaDoc;  // Esquema to use
    QString              m_csvFormat; // A string made of csv each having the name of a formula or static data. this data will be replaced with the extracted data

public:
    // Functions to add and delete PDF file paths
    void addPdfFile(const QString& path)             { m_pdfFilePaths.push_back(path); }
    const std::vector<QString>& getFilePaths() const { return m_pdfFilePaths; }
    void deletePdfFile(int index);

    // Function to reorder files inside the vector
    void reOrderFiles(int fileToMoveIndex, int targetPositionIndex);

    // Getters and setters
    CEsquemaDoc* getAsocEsquemaDoc() const    { return m_associatedEsquemaDoc; }
    void setAsocEsquemaDoc(CEsquemaDoc* pDoc) { m_associatedEsquemaDoc = pDoc; }

    void setCSVFormat(const QString& string) { m_csvFormat = string; }
    const QString& getCSVFormat()            { return m_csvFormat; }

    std::vector<QString> getPathFiles() const { return m_pdfFilePaths; }
    void setPathFiles(const std::vector<QString>& paths) {
        m_pdfFilePaths.clear();
        m_pdfFilePaths = paths;
    }

    // Public function to build XSV structure
    std::vector<std::vector<CData*>> buildXSVStructure(ExportCSVProgressBar_dlg *progressDialog = nullptr);
};

#endif // CEXPORTCSV_H
