// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#ifndef CESQUEMADOC_H
#define CESQUEMADOC_H

#include "CDocument.h"

class CEsquemaDoc : public CDocument {
public:
    // Constructors and destructors
    CEsquemaDoc(CEsquema* esquema);
    ~CEsquemaDoc() override;

    // Getters
    CEsquema* getEsquema() const;

protected:
    // Members
    CEsquema* m_esquema = nullptr; // The associated esquema to this doc.
};


#endif // CESQUEMADOC_H
