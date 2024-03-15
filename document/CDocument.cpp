// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "CDocument.h"
#include "CMDoc.h"
CDocument::CDocument() {}

CDocument::~CDocument() {
    CMDoc::getMDoc().onDocumentDestroyed(this);
}
