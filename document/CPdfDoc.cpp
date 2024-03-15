// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "CPdfDoc.h"

#include <QFile>

CPdfDoc::CPdfDoc(const QString& filePath) : m_fullText(""), m_filePath(filePath) {
    m_doc = poppler::document::load_from_file(m_docName.toStdString());
    const int pagesNbr = m_doc->pages();

    QString line("");
    QString formatedLine("");
    for (int i = 0; i < pagesNbr; ++i) {
        line += m_doc->create_page(i)->text().to_utf8().data();
        m_pages.push_back(std::move(line));

        bool previousSpace  = false; // Flag to track if the previous character was a space
        bool firstChar      = true;  // Flag to track if the current character is the first in the line

        for (int j = 0; j < line.length(); ++j) {
            QChar c = line.at(j);        // Check for newline characters (CR or LF)
            if (c == '\n' || c == '\r') {
                if (!formatedLine.isEmpty() && formatedLine.back() == '\n') continue; // Skip consecutive newline characters
                formatedLine.append('\n');
                previousSpace = false;
                firstChar     = true;
            }
            // Check for whitespace characters (space, tab, etc.)
            else if (c.isSpace()) {
                if (firstChar) continue; // Skip consecutive spaces
                if (!previousSpace) {
                    formatedLine.append(' ');
                    previousSpace = true; // Set the previous space flag
                }
            }
            else { // For non-whitespace characters
                formatedLine.append(c);
                previousSpace = false;
                firstChar     = false;
            }
        }
    }
}

CPdfDoc::~CPdfDoc() {
    delete m_doc;
}

const QString &CPdfDoc::getFilePath() const {
    return m_filePath;
}

const QString& CPdfDoc::getFullText() {
    if (m_fullText.isEmpty()) {
        for(const QString& page : m_pages) {
            m_fullText.append(page);
        }
    }
    return m_fullText;
}

const QString& CPdfDoc::getPage(const int index) {
    if (index < 0 || index >= m_pages.size()) {
        qDebug() << "Index out of bound for page of document";
    }
    return m_pages.at(index);
}
