/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WToolBarPreview.h"


WToolBarPreview::WToolBarPreview(QWidget *parent) : QToolBar(parent), m_toggleButtonsGroup(this),
    m_textPreview("Text Preview", this), m_resultPreview("Result Preview", this), m_spreadSheetPreview("CSV Preview", this) {

    // Adds tooglable button actions to the toolbar and sets the first one checked.
    m_textPreview.setCheckable(true);
    m_textPreview.setChecked(true);
    m_toggleButtonsGroup.addAction(&m_textPreview);
    addAction(&m_textPreview);

    m_resultPreview.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_resultPreview);
    addAction(&m_resultPreview);

    m_spreadSheetPreview.setCheckable(true);
    m_toggleButtonsGroup.addAction(&m_spreadSheetPreview);
    addAction(&m_spreadSheetPreview);

    connect(&m_toggleButtonsGroup, &QActionGroup::triggered, this, &WToolBarPreview::handleActionTriggered);
}

void WToolBarPreview::handleActionTriggered(QAction *action) {
    if      (action == &m_textPreview)          emit previewOptionChanged(PreviewOption::TextPreview);
    else if (action == &m_resultPreview)        emit previewOptionChanged(PreviewOption::ResultPreview);
    else if (action == &m_spreadSheetPreview)   emit previewOptionChanged(PreviewOption::CSVPreview);
}
