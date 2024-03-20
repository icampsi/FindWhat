/* =================================================== *
 * ====        Copyright (c) 2024 icampsi       ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CPdfDoc.h"
#include <QFileInfo>

#include <QFile>

CPdfDoc::CPdfDoc(const QString& filePath) : m_fullText(""), m_filePath(filePath) {
    QFileInfo fileInfo(m_filePath);
    m_docName = fileInfo.fileName();

    // Loads PDF file into poppler
    m_doc = poppler::document::load_from_file(m_filePath.toStdString());
    const int pagesNbr = m_doc->pages(); // Number of pages
    size_t charCount = 0;
    // Extracts text page by page and stores it inside m_pages
    for (int i = 0; i < pagesNbr; ++i) {
        QString line("");
        Page newPage{"", {0, 0}};
        line += m_doc->create_page(i)->text().to_utf8().data();

        bool previousSpace  = false; // Flag to track if the previous character was a space
        bool firstChar      = true;  // Flag to track if the current character is the first in the line (not counting blank characters like whitespaces)

        // Parses extracted text to destroy multiple spacings and stacked new lines
        for (int j = 0; j < line.length(); ++j) {
            QChar c = line.at(j);        // Check for newline characters (CR or LF)
            if (c == '\n' || c == '\r') {
                if (!newPage.pageText.isEmpty() && newPage.pageText.back() == '\n') continue; // Skip consecutive newline characters
                newPage.pageText.append('\n');
                previousSpace = false;
                firstChar     = true;
            }
            // Check for whitespace characters (space, tab, etc.)
            else if (c.isSpace()) {
                if (firstChar) continue; // Skip consecutive whitespaces
                if (!previousSpace) {
                    newPage.pageText.append(' ');
                    previousSpace = true; // Set the previous space flag
                }
            }
            else { // For non-whitespace characters
                newPage.pageText.append(c);
                previousSpace = false;
                firstChar     = false;
            }
        }
        newPage.pageCharRange = {charCount, newPage.pageText.length() - 1 + charCount};
        charCount += newPage.pageText.length();
        m_pages.push_back(std::move(newPage)); // Saves page individually to acces specific pages later
    }
    m_textSize = --charCount;
}

CPdfDoc::~CPdfDoc() {
    delete m_doc;
}

const QString &CPdfDoc::getFilePath() const {
    return m_filePath;
}

const QString& CPdfDoc::getFullText() {
    if (m_fullText.isEmpty()) { // First time this is called poppulates m_fullText by appending every page text
        for(const Page& page : m_pages) {
            m_fullText.append(page.pageText);
        }
    }
    return m_fullText;
}

const CPdfDoc::Page& CPdfDoc::getPage(const size_t index) const {
    if (index >= m_pages.size()) {
        qWarning() << "Index out of bound for pages of document";
    }
    return m_pages.at(index);
}

const CPdfDoc::RelativeIndex CPdfDoc::getPageRelativeIndex(const size_t index) {
    RelativeIndex relativeIndex{};
    for(size_t i{0}; i < m_pages.size(); i++) {
        if(index >= m_pages.at(i).pageCharRange.from && index < m_pages.at(i).pageCharRange.to) {
            relativeIndex.pageIndex = i;
            relativeIndex.charIndex = index - m_pages.at(i).pageCharRange.from;
            break;
        }
    }
    return relativeIndex;
}

size_t CPdfDoc::calculateComprehensiveIndex(const size_t index, const size_t page) const {
    return index + m_pages.at(page).pageCharRange.from;
}

