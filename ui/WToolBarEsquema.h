/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WTOOLBARESQUEMA_H
#define WTOOLBARESQUEMA_H

#include "qactiongroup.h"
#include "qtoolbar.h"
#include <QObject>

class WToolBarEsquema : public QToolBar {
    Q_OBJECT
public:
    explicit WToolBarEsquema(QWidget* parent);
    enum class EsquemaOption { EditEsquema, ParseDocument };

signals:
    void previewOptionChanged(WToolBarEsquema::EsquemaOption option);

private:
    QActionGroup m_toggleButtonsGroup;
    QAction m_editEsquema;
    QAction m_parseDoc;
};

#endif // WTOOLBARESQUEMA_H
