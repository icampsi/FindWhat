/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CESQUEMA_H
#define CESQUEMA_H

#include <QString>
#include <QMessageBox>
#include <fstream>

#include "CData.h"
#include "CFormula.h"

class CData;
class CPdfDoc;

// Stores all necessary informacion for extracting specified values from a PDF and make CData from each
class CEsquema {
public:
    // CONSTRUCTORS & DESTRUCTORS
    CEsquema(const QString& nameEsquema, const std::vector<CFormula *> &tExtractDataFormula, const std::vector<CData*>& valorsEstatics);
    CEsquema(const QString &nameEsquema, const std::vector<CFormula *> &tExtractDataFormula);
    CEsquema(std::ifstream& in) { deserliazile(in); }
    CEsquema() = default;
    ~CEsquema();

protected:
    // MEMBERS
    QString m_nameEsquema; // Name for the esquema

    std::vector<CFormula*> m_extractDataFormula;         // Formula vector for each data to be extracted
    std::vector<CData*>    m_staticData;                 // Static values for the esquema. They are not extracting text from a file but rather youcreate a variable with a fixed string attached
    std::vector<QString>   m_csvFormatFormulaStructure;  // { <dataName1>,<dataName2>,some_fixed_text,,,<dataName3> etc. } Specifies order of cells in the xsv exported file. Still want to add an option for "\n" for new row

    std::vector<std::vector<QString>> m_XSVStringStructureResult; // Stores the result as an xsv string structure

    std::vector<size_t> m_dataForCheck1; // { 8, 9, 5} For the future. Índex dels valors que volem fer check. val1 + val2 == val 3? (molaria donar més opcions d'operacions complexes)
    std::vector<size_t> m_dataForCheck2; // For the future. Índex dels valors que volem fer check. L'últim és el valor a comparar.

    QMap<QString, CData*>  m_dataMap;             // This maps every CData structure with it's name for efficient lookup. This includes also data from inside every CFormula in the t_extractDataFormula

public:
    void renameFile(const char* oldName, const char* newName);
    bool createFileName(QString& newFileName, const QString& fileNamePlaceholder) const; // Return false if name contains any invalid naming character: < > " \ / | ? *

    // Generates a string xsv Structure from the format specified in m_csvFormatFormula
    void generateXSVStringStructure(CPdfDoc *pPdfDoc);
    // Converts a xsv string structure into a full csv file string
    void xsv_stringStructureToString(QString* pFullFileString, char enclosureChar, char separator);
    // Deletes the formula at index pos and cleans
    void deleteFormula(const size_t index);

    // SETTERS AND GETTERS
    void addStaticData(CData* data );
    const std::vector<CData*>& getStaticData() const { return m_staticData; }
    void deleteStaticData(size_t index);
    void setStaticDataName(CData* data, const QString &name);

    const std::vector<CFormula*>& getExtractDataFormula() const { return m_extractDataFormula; }

    void addExtractDataFormula(CFormula* formula) {
        m_dataMap.insert(formula->getDataName(), formula->getData());
        m_extractDataFormula.push_back(formula);
    }

    void setFormulaName(CFormula* formula, const QString &name);

    const QString& getName() const      { return m_nameEsquema; }
    void setName(const QString& name)   { m_nameEsquema = name; }

    const std::vector<std::vector<QString>>& getXSVStringStructureResult() const { return m_XSVStringStructureResult; };

    // Breaks a csv line into a vector to set up m_csvFormatFormula.
    void constructCsvFormatFormulaStructure(const QString& rLine, char enclosureChar, char separator);
    void setCsvFormatFormula(const std::vector<QString>& format) { m_csvFormatFormulaStructure = format; }

    // SERIALIZATION
    void serialize(std::ofstream& out) const;
    void deserliazile(std::ifstream& in);

    // FUTURE UPDATES:
    // bool checkData(CFileData& file); // Still unused
    // void inline formatDate(QString& data);  // Still unused
};

#endif // CESQUEMA_H
