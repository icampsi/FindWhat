// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "WToolBarEsquema.h"

WToolBarEsquema::WToolBarEsquema(QWidget* parent) : QToolBar(parent), m_toggleButtonsGroup(this),
    m_editEsquema("Edit Esquema", this), m_parseDoc("Parse Document", this) {

    m_editEsquema.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_editEsquema);
    addAction(&m_editEsquema);

    m_parseDoc.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_parseDoc);
    addAction(&m_parseDoc);
    connect(&m_toggleButtonsGroup, &QActionGroup::triggered, this, &WToolBarEsquema::handleActionTriggered);
    m_editEsquema.trigger();
}

void WToolBarEsquema::handleActionTriggered(QAction *action) {
    if      (action == &m_editEsquema) emit previewOptionChanged(EsquemaOption::EditEsquema);
    else if (action == &m_parseDoc)    emit previewOptionChanged(EsquemaOption::ParseDocument);
}
