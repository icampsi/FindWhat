/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WToolBarEsquema.h"

WToolBarEsquema::WToolBarEsquema(QWidget* parent) : QToolBar(parent), m_toggleButtonsGroup(this),
    m_editEsquema("Edit Esquema", this), m_parseDoc("Parse Document", this)
#ifdef ENABLE_DBMANAGER
    , m_dbManager("Manage DB", this)
#endif
{
    m_editEsquema.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_editEsquema);
    addAction(&m_editEsquema);

    m_parseDoc.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_parseDoc);
    addAction(&m_parseDoc);

#ifdef ENABLE_DBMANAGER
    m_dbManager.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_dbManager);
    addAction(&m_dbManager);
#endif

    connect(&m_toggleButtonsGroup, &QActionGroup::triggered, this, [=](QAction* action) {
        if      (action == &m_editEsquema) emit optionChanged(EsquemaOption::EditEsquema);
        else if (action == &m_parseDoc   ) emit optionChanged(EsquemaOption::ParseDocument);
#ifdef ENABLE_DBMANAGER
        else if(action == &m_dbManager   ) emit optionChanged(EsquemaOption::DbManager);
#endif
    });

    m_editEsquema.trigger();
}
