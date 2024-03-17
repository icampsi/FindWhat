/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CDOCUMENT_H
#define CDOCUMENT_H

#include "src/CEsquema.h"

class CMDoc;

class CDocument {
public:
    // Constructors and destructors
    CDocument();
    CDocument(const QString& docName);
    virtual ~CDocument();

protected:
    // Members
    QString m_docName = "";

public:
    // Getters and setters
    QString getDocName()             { return m_docName; }
    void setDocName(QString docName) { m_docName = docName; }
};

#endif // CDOCUMENT_H
