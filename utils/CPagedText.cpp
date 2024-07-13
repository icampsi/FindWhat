/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CPagedText.h"

#include <QDebug>

CPagedText::CPagedText(const std::vector<QString>& pages, bool normalizeText) {
    for(const QString& page : pages) {
        push_back(page, normalizeText);
    }
}

void CPagedText::normalizePageText(const QString& rawText, QString *normalizedTxt) const {
    bool previousSpace = false;
    bool firstChar = true;

    for (int j = 0; j < rawText.length(); ++j) {
        QChar c = rawText.at(j);

        if (c == '\n' || c == '\r') {
            if (!normalizedTxt->isEmpty() && (normalizedTxt->back() == '\n' || normalizedTxt->back() == '\r'))
                continue; // Skip consecutive newline characters
            else if (normalizedTxt->isEmpty())
                continue; // Avoids starting with a new line
            normalizedTxt->append('\n');
            previousSpace = false;
            firstChar = true;
        } else if (c.isSpace()) {
            if (firstChar)
                continue; // Skip consecutive whitespaces
            if (!previousSpace) {
                normalizedTxt->append(' ');
                previousSpace = true;
            }
        } else {
            normalizedTxt->append(c);
            previousSpace = false;
            firstChar = false;
        }
    }
    *normalizedTxt = normalizedTxt->trimmed();
}

void CPagedText::populateFullText() {
    // First time this is called, populates m_fullText by appending every page text
    if (m_fullText.isEmpty()) {
        for (const Page& page : m_pages) {
            m_fullText.append(page.pageText);
        }
    }
}

QString& CPagedText::getFullTextNonConst() {
    populateFullText();
    return m_fullText;
}

const CPagedText::Page& CPagedText::getPage(const size_t index) const {
    if (index >= m_pages.size()) {
        qWarning() << "Index out of bound for pages of document";
    }
    return m_pages.at(index);
}

const CPagedText::RelativeIndex CPagedText::getPageRelativeIndex(const size_t index) const {
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

size_t CPagedText::calculateComprehensiveIndex(const size_t index, const size_t pageNum) const {
    size_t relativeIndex = 0;
    for(size_t i{0}; i < pageNum; i++) {
        relativeIndex += m_pages.at(i).pageText.size();
    }
    if(pageNum != 0)
        relativeIndex -= 1;
    return relativeIndex + index;
}

void CPagedText::push_back(const QString& pageText, bool normalizeText) {
    if(normalizeText) {
        QString normalizedText;
        normalizePageText(pageText, &normalizedText);
        Page page{normalizedText, PageCharRange{calculateComprehensiveIndex(0, m_pages.size()), calculateComprehensiveIndex(pageText.size() - 1, m_pages.size())}}; // NEW BOOKRMARK - POSSIBLE ERROR BECAUSE TRYING TO ACCES NON EXISTING PAGE INSIDE CALCULATE FUNCTION
        m_pages.push_back(std::move(page));
    } else {
        m_pages.push_back(Page{pageText, PageCharRange{calculateComprehensiveIndex(0, m_pages.size()), calculateComprehensiveIndex(pageText.size() - 1, m_pages.size())}}); // NEW BOOKRMARK - POSSIBLE ERROR BECAUSE TRYING TO ACCES NON EXISTING PAGE INSIDE CALCULATE FUNCTION
    }
    if(!m_fullText.isEmpty()) { // If full text has already been populated, add the page there as well
        m_fullText.append(pageText);
    }
}
