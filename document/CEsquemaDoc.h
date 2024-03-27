/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CESQUEMADOC_H
#define CESQUEMADOC_H

#include "CDocument.h"

class CEsquemaDoc : public CDocument {
public:
    // CONSTRUCTORS&DESTRUCTORS
    CEsquemaDoc(CEsquema* esquema);
    ~CEsquemaDoc() override;

    // GETTERS
    CEsquema* getEsquema() const { return m_esquema; }

protected:
    // MEMBERS
    CEsquema* m_esquema = nullptr; // The associated esquema to this doc.

    // SERIALIZATION

};

#endif // CESQUEMADOC_H
