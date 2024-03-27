/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CEXPORTPATHDOC_H
#define CEXPORTPATHDOC_H

#include "CExportCSV.h"

#include <vector>
#include <QTextStream>

// Document to hold all the information for the export path to follow
class CExportPathDoc {
public:
    //Constructors and destructors
    CExportPathDoc() : m_exportCSVs(){}
    ~CExportPathDoc();

private:
    std::vector<CExportCSV*> m_exportCSVs;

public:
    // INTERFACE FOR m_exportCSVs
    // Create a new CExportCSV object and return a pointer to it
    CExportCSV* createExportCSV() {
        m_exportCSVs.emplace_back(new CExportCSV());
        return m_exportCSVs.back();
    }

    // Get the size of the vector
    size_t getExportCSVCount() const { return m_exportCSVs.size(); }

    // Get a pointer to a specific CExportCSV object by index
    CExportCSV* getExportCSVByIndex(size_t index);
    const std::vector<CExportCSV*>& getExportCSVs() { return m_exportCSVs; }
    void deleteExportCSV(size_t index);

    // Creates a .csv File from a xsvStringStructure
    void xsvm_stringStructureToFile(const QString& fileName, std::vector<std::vector<QString>>& rXSVStructure, QChar separator);

    // SERIALIZATION
    void serialize(std::ofstream &out) const;
    void deserialize(std::ifstream &in);
};
#endif // CEXPORTPATHDOC_H
