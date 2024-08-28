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
    //CONSTRUCTORS & DESTRUCTORS
    CExportPathDoc() : m_exportCSVs(){}
    ~CExportPathDoc();

protected:
    // MEMBERS
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
    CExportCSV* getExportCSVByIndex(size_t index) const;
    const std::vector<CExportCSV*>& getExportCSVs() const { return m_exportCSVs; }
    void deleteExportCSV(size_t index);

    // Returns the colective size of the stored exportCSV filePaths
    size_t getFileCount();

    // Creates a .csv File from a QStandardItemModel
    void modelToFile(const QString& fileName, QAbstractItemModel *model);

    // SERIALIZATION
    void serialize(std::ofstream &out) const;
    void deserialize(std::ifstream &in);
};
#endif // CEXPORTPATHDOC_H
