// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "CExportPathDoc.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

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

size_t CExportPathDoc::getExportCSVCount() const {
    return m_exportCSVs.size();
}

CExportCSV* CExportPathDoc::getExportCSVByIndex(size_t index) {
    if (index >= m_exportCSVs.size()) {
        return nullptr; // Index out of range
    }
    return m_exportCSVs[index];
}
