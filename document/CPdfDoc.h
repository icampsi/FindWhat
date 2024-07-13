/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CPDFDOC_H
#define CPDFDOC_H

#include "CDocument.h"
#include "CPagedText.h"

class CPdfDoc : public CDocument {

public:
    // CONSTRUCTORS & DESTRUCTORS
    explicit CPdfDoc(const QString& filePath);
    //GETTERS (No need for setters, it should all be assigned on construction and never changed)
    const QString& getFilePath() const;
    const CPagedText& getPagedText() const
    { return m_doc; }
    const QString& getFullText()
    { return m_doc.getFullText(); }

private:
    void initializeFileInfo();
    void loadPdfDocument();

protected:
    // MEMBERS
    QString m_filePath;        // Path to pdf file
    CPagedText m_doc;
};

#endif // CPDFDOC_H
