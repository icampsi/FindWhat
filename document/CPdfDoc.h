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
public:
    // CONSTRUCTORS & DESTRUCTORS
    CPdfDoc(const QString& filePath);
    ~CPdfDoc();

    //GETTERS (No need for setters, it should all be assigned on construction and never changed)
    const QString& getFilePath() const;
    const QString& getFullText();
    const QString& getPage(const size_t index);

protected:
    // MEMBERS
    QString m_fullText;             // Appended full text (not divided by pages). Gets poppulated first time getFullText is called.
    QString m_filePath;             // Path to pdf file
    std::vector<QString> m_pages;   // Pdf extracted text divided in pages. Created and populated on constructor
    poppler::document* m_doc;       // Loads it from static constructor
};

#endif // CPDFDOC_H
