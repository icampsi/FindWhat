/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PMAINESQUEMAUI_H
#define PMAINESQUEMAUI_H

#include <QWidget>
#include <QStackedWidget>

#include "WToolBarEsquema.h"
#include "PEsquemaPage.h"
#include "ui_PMainEsquemaUI.h"

class CExportCSV;

namespace Ui { class PMainEsquemaUI; }

// Edition interface for esquema. Each esquema would have it's own instance of this widget
class PMainEsquemaUI : public QWidget {
    Q_OBJECT

public:
    // CONSTRUCTORS & DESTRUCTORS
    explicit PMainEsquemaUI(QWidget *parent = nullptr);
    ~PMainEsquemaUI();

    // PUBLIC FUNCTIONS
    void newEsquema(CEsquemaDoc* esquemaDoc);
    void changeCurrentPage(PEsquemaPage* page) { ui->stackedWidget_esquemaPage->setCurrentWidget(page); }
    void addExportCSV(CExportCSV *exportCSV);

private slots:
    void esquemaOptionChanged(WToolBarEsquema::EsquemaOption option) { ui->stackedWidget_esquemaUI->setCurrentIndex(static_cast<int>(option)); }
    void on_pushButton_addPage_clicked();
    void on_DeleteEsquema_clicked();
    void on_pushButton_ExportCSV_clicked();

public slots:
    void handleDeleteEsquema(const size_t index);
    void handleFilePathChanged(const QString &filePath);
    void setCurrentPageToEmptyPage() { ui->stackedWidget_esquemaPage->setCurrentWidget(&m_emptyPage); } // Sets the current page to empty page in case no esquemas are loaded

private:
    Ui::PMainEsquemaUI *ui;
    PEsquemaPage m_emptyPage; // empty page to load when no esquema is loaded
};
#endif // PMAINESQUEMAUI_H
