/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CEsquemaDoc.h"

CEsquemaDoc::CEsquemaDoc(CEsquema* esquema) : m_esquema{esquema} {}

CEsquemaDoc::~CEsquemaDoc()  {
    delete m_esquema;
    m_esquema = nullptr;
}

CEsquema* CEsquemaDoc::getEsquema() const { return m_esquema; }
