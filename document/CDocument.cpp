/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CDocument.h"
#include "CMDoc.h"
CDocument::CDocument() {}

CDocument::CDocument(const QString& docName) : m_docName{docName} {}

CDocument::~CDocument() {
    CMDoc::getMDoc().onDocumentDestroyed(this);
}
