/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CExportPathDoc.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

// EXPORTPATHDOC
CExportPathDoc::~CExportPathDoc() {
    for (auto* exportCSV : m_exportCSVs) {
        delete exportCSV;
    }
}

CExportCSV* CExportPathDoc::createExportCSV() {
    m_exportCSVs.emplace_back(new CExportCSV());
    return m_exportCSVs.back();
}

CExportCSV* CExportPathDoc::getExportCSVByIndex(size_t index) {
    if (index >= m_exportCSVs.size()) {
        return nullptr; // Index out of range
    }
    return m_exportCSVs[index];
}

void CExportPathDoc::deleteExportCSV(size_t index) {
    // Check if the index is within valid range
    if (index >= m_exportCSVs.size()) {
        qDebug() << "Invalid index:" << index;
        return;
    }

    // Delete the item at the specified index
    delete m_exportCSVs[index];
    m_exportCSVs.erase(m_exportCSVs.begin() + index);
}

void CExportPathDoc::xsvm_stringStructureToFile(const QString& fileName, std::vector<std::vector<QString>>& rXSVStructure, QChar separator) {
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
