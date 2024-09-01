/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CESQUEMA_H
#define CESQUEMA_H

#include "CPagedText.h"
#include <QString>
#include <QMessageBox>

class CFormula;
class CData;
class CPdfDoc;

// Stores all necessary information for extracting specified values from a PDF and make CData from each
class CEsquema {
public:
    // CONSTRUCTORS & DESTRUCTORS
    CEsquema(std::ifstream& in) { deserliazile(in); }
    CEsquema() = default;
    ~CEsquema();

protected:
    // MEMBERS
    QString m_nameEsquema; // Name for the esquema

    std::vector<CFormula*> m_extractDataFormula; // Formula vector for each data to be extracted
    std::vector<CData*>    m_staticData;         // Static values for the esquema. They are not extracting text from a file but rather you create a variable with a fixed string attached

    QMap<QString, CData*>  m_dataMap; // This maps every CData structure with it's name for efficient lookup. This includes also data from inside every CFormula in the t_extractDataFormula

public:
    void renameFile(const char* oldName, const char* newName);
    bool createFileName(QString& newFileName, const QString& fileNamePlaceholder) const; // Return false if name contains any invalid naming character: < > " \ / | ? *

    // Deletes the formula at index pos and cleans
    void deleteFormula(const size_t index);

    // SETTERS AND GETTERS
    void addStaticData(CData* data );
    const std::vector<CData*>& getStaticData() const { return m_staticData; }
    void deleteStaticData(size_t index);
    void setStaticDataName(CData* data, const QString &name);

    const std::vector<CFormula*>& getExtractDataFormula() const { return m_extractDataFormula; }

    void addExtractDataFormula(CFormula* formula);

    void setFormulaName(CFormula* formula, const QString &name);

    const QString& getName() const    { return m_nameEsquema; }
    void setName(const QString& name) { m_nameEsquema = name; }

    // SERIALIZATION
    void serialize(std::ofstream& out) const;
    void deserliazile(std::ifstream& in);
};

#endif // CESQUEMA_H
