/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CPDFDOC_H
#define CPDFDOC_H

#include "CDocument.h"

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"

class CPdfDoc : public CDocument {
protected:
    struct PageCharRange { size_t from = 0; size_t to = 0; };


public:
    struct RelativeIndex {
        size_t pageIndex = 0;
        size_t charIndex = 0;
    };
    struct Page {
        QString       pageText;
        PageCharRange pageCharRange; // Keeps count from where to where goes the page index related to the full document
    };

public:
    // CONSTRUCTORS & DESTRUCTORS
    CPdfDoc(const QString& filePath);
    ~CPdfDoc();

    //GETTERS (No need for setters, it should all be assigned on construction and never changed)
    const QString& getFilePath() const;
    const QString& getFullText();
    const Page &getPage(const size_t index) const;

    size_t pageCount() { return m_pages.size(); }
    // Find what page correspon at what index from the full document text and returns its relative index along with the page number
    const RelativeIndex getPageRelativeIndex(const size_t index);
    // Gets the total index from the relative index of a page
    size_t calculateComprehensiveIndex(const size_t index, const size_t page) const;

protected:
    // MEMBERS
    QString m_fullText;        // Appended full text (not divided by pages). Gets poppulated first time getFullText is called.
    QString m_filePath;        // Path to pdf file
    long long m_textSize;
    std::vector<Page> m_pages; // Pdf extracted text divided in pages. Created and populated on constructor
    poppler::document* m_doc;  // Loads it from static constructor
};

#endif // CPDFDOC_H
