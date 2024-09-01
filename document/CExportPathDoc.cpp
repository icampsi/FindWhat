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
#include <fstream>

// EXPORTPATHDOC
CExportPathDoc::~CExportPathDoc() {
    for (auto* exportCSV : m_exportCSVs) {
        delete exportCSV;
    }
}

CExportCSV* CExportPathDoc::getExportCSVByIndex(size_t index) const {
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

size_t CExportPathDoc::getFileCount() {
    size_t fileCount = 0;
    for (CExportCSV* it : m_exportCSVs) {
        fileCount += it->getFiles().size();
    }
    return fileCount;
}


void CExportPathDoc::modelToFile(const QString& fileName, QAbstractItemModel *model) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, QStringLiteral("ERROR"), QStringLiteral("Couldn't create file"));
        return;
    }

    QTextStream out(&file);

    const int rowCount = model->rowCount();
    const int columnCount = model->columnCount();

    // Iterate through rows and columns of the model
    for (int row = 0; row < rowCount; ++row) {
        QStringList rowData;
        for (int col = 0; col < columnCount; ++col) {
            // Get data from model
            QModelIndex index = model->index(row, col);
            QString data = model->data(index).toString();
            // Replace any commas in the data to avoid CSV format issues
            if(data.contains(',')) {
                data.append("\"");
                data.prepend("\"");
            }
            // Append the data to the row list
            rowData.append(data);
        }
        // Join the row data with commas and write to the file
        out << rowData.join(",") << "\n";
    }
    QMessageBox::information(nullptr, "Succes!", ".csv file created succesfully");
}

// SERIALIZATION
void CExportPathDoc::serialize(std::ofstream &out) const {
    size_t m_exportCSVsSize = m_exportCSVs.size();
    out.write(reinterpret_cast<const char*>(&m_exportCSVsSize), sizeof(size_t));

    for(CExportCSV *exportCSV : m_exportCSVs) {
        exportCSV->serialize(out);
    }
}

void CExportPathDoc::deserialize(std::ifstream &in) {
    size_t m_exportCSVsSize;
    in.read(reinterpret_cast<char*>(&m_exportCSVsSize), sizeof(size_t));

    for(size_t i{ 0 }; i < m_exportCSVsSize; i++) {
        CExportCSV *exportCSV = new CExportCSV(in);
        m_exportCSVs.push_back(exportCSV);
    }
}
