/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CPdfDoc.h"
#include "../poppler_interface/PdfToText.h"

#include <QFileInfo>
#include <QFile>

CPdfDoc::CPdfDoc(const QString& filePath) : m_filePath(filePath), m_doc() {
    initializeFileInfo();
    Poppler_interface::loadPdfDocument(m_filePath, &m_doc);
}

void CPdfDoc::initializeFileInfo() {
    QFileInfo fileInfo(m_filePath);
    m_docName = fileInfo.fileName();
}

const QString &CPdfDoc::getFilePath() const {
    return m_filePath;
}
