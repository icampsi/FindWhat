/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CPAGEDTEXT_H
#define CPAGEDTEXT_H

#include <QString>

class CPagedText {
protected:
    struct PageCharRange {
        size_t from = 0;
        size_t to = 0;
    };

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
    CPagedText(const std::vector<QString>& pages, bool normalizeText = false);
    CPagedText(const QString& text, bool normalizeText = false)
    { push_back(text, normalizeText); }
    CPagedText() // Default constructor
        : m_fullText(""), m_pages() {}

private:
    // PRIVATE METHODS
    // Eliminates multiple spaces, tabs and new lines.
    void normalizePageText(const QString& rawText, QString *normalizedTxt) const;
    // Populates m_fullText (called first time getFullText is used)
    void populateFullText();

    //GETTERS & SETTERS
    QString& getFullTextNonConst();

public:
    // Cast away constness and call the non-const method
    const QString& getFullText() const
    { return const_cast<CPagedText*>(this)->getFullTextNonConst(); }

    // Return page by index
    const Page &getPage(const size_t index) const;
    // Return total number of pages
    size_t pageCount() const
    { return m_pages.size(); }
    // Find what page correspon at what index from the full document text and returns its relative index along with the page number
    const RelativeIndex getPageRelativeIndex(const size_t index) const;

    // Gets the total index from the relative index of a pageNum
    size_t calculateComprehensiveIndex(const size_t index, const size_t pageNum) const;

    // BOOKMARK - Does the same as getComprehensiveIndex(), less efficiently but without needing pageCharRange. I am not deleating it cause i want to get rid of pageCharRange
    // size_t calculateComprehensiveIndex(const size_t index, const size_t pageNum) const
    // { return index + m_pages.at(pageNum).pageCharRange.from; }

    // Returns all pages in a vector
    const std::vector<Page>& getPagedDoc() { return m_pages; }
    // typical push_back interface for pages and a normalizePageText() option.
    void push_back(const QString& pageText, bool normalizeText = false );

    // BOOKMARK - NOT IMPLEMENTED YET
    void deletePage(const size_t index)
    { Q_UNUSED(index); } // Not implemented
    void insertPage(const QString& page, const size_t index)
    { Q_UNUSED(page); Q_UNUSED(index); } // Not implemented

protected:
    // MEMBERS
    QString m_fullText;        // Appended full text (not divided by pages). Gets poppulated first time getFullText is called.
    std::vector<Page> m_pages; // Pdf extracted text divided in pages. Created and populated on constructor
};

#endif // CPAGEDTEXT_H
