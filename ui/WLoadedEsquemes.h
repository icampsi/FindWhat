/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WLOADEDESQUEMES_H
#define WLOADEDESQUEMES_H

#include <QWidget>
#include "PEsquemaPage.h"
#include "ui_WLoadedEsquemes.h"
#include <QMap>

namespace Ui { class WLoadedEsquemes; }

class WLoadedEsquemes : public QWidget {
    Q_OBJECT

public:
    explicit WLoadedEsquemes(QWidget *parent = nullptr);
    ~WLoadedEsquemes();
    void newEsquema(PEsquemaPage *page, CEsquema *esquema);
    void deleteEsquema(const size_t index) { ui->list_esquemes->handleDeleteEsquema(index); }

private slots:
    void on_list_esquemes_itemSelectionChanged();

private:
    Ui::WLoadedEsquemes *ui;
    QMap<QListWidgetItem*, PEsquemaPage*> m_itemPageMap;
};

#endif // WLOADEDESQUEMES_H
