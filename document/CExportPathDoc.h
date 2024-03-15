// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#ifndef CEXPORTPATHDOC_H
#define CEXPORTPATHDOC_H

#include "CExportCSV.h"
#include <vector>

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

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
    CExportCSV* createExportCSV();

    // Get the size of the vector
    size_t getExportCSVCount() const;

    // Get a pointer to a specific CExportCSV object by index
    CExportCSV* getExportCSVByIndex(size_t index);
    const std::vector<CExportCSV*>& getExportCSVs() { return m_exportCSVs; }
    void deleteExportCSV(int index) {
        // Check if the index is within valid range
        if (index < 0 || index >= m_exportCSVs.size()) {
            qDebug() << "Invalid index:" << index;
            return;
        }

        // Delete the item at the specified index
        delete m_exportCSVs[index];
        m_exportCSVs.erase(m_exportCSVs.begin() + index);
    }

    // Creates a .csv File from a xsvStringStructure
    void xsvm_stringStructureToFile(const QString& fileName, std::vector<std::vector<QString>>& rXSVStructure, QChar separator) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(nullptr, QStringLiteral("ERROR"), QStringLiteral("Couldn't create file"));
            return;
        }

        QTextStream out(&file);
        for (const auto& row : rXSVStructure) {
            for (const auto &cell : row) {
                out << '"';
                for (const QChar& c : cell) {
                    if (c == '"')
                        out << "\"\"";
                    else
                        out << c;
                }
                out << '"' << separator;
            }
            out << '\n';
        }
        QMessageBox::information(nullptr, "Succes!", ".csv file created succesfully");
    }
};
#endif // CEXPORTPATHDOC_H
