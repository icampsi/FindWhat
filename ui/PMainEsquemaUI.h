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

QT_BEGIN_NAMESPACE
namespace Ui { class PMainEsquemaUI; }
QT_END_NAMESPACE

// Edition interface for esquema. Each esquema would have it's own instance of this widget
class PMainEsquemaUI : public QWidget {
    Q_OBJECT

public:
    // CONSTRUCTORS & DESTRUCTORS
    explicit PMainEsquemaUI(QWidget *parent = nullptr);
    ~PMainEsquemaUI();

    // PUBLIC FUNCTIONS
    void newEsquema(CEsquema* esquema);
    void changeCurrentPage(PEsquemaPage* page) { ui->stackedWidget_esquemaPage->setCurrentWidget(page); }
    void addExportCSV(CExportCSV *exportCSV);
    void deletePage(int index); // Deletes a page from the toobox
    int  pageCount() { return ui->toolBox_formatEsquema->count(); }
    void clearPages(); // Clears all pages (calls deletePage one by one)
    void deleteEsquema(const int index, const bool askConfirmation = false);

private slots:
    void esquemaOptionChanged(WToolBarEsquema::EsquemaOption option) { ui->stackedWidget_esquemaUI->setCurrentIndex(static_cast<int>(option)); }
    void addPage() { addExportCSV(nullptr); }
    void on_pushButton_parse_clicked();
    void handleEsquemaSelectionChanged();
    void handleEsqItemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

public slots:
    void handleDeleteEsquema(const int index);
    void handleFilePathChanged(const QString &filePath);
    void setCurrentPageToEmptyPage() { ui->stackedWidget_esquemaPage->setCurrentWidget(&m_emptyPage); } // Sets the current page to empty page in case no esquemas are loaded

private:
    Ui::PMainEsquemaUI *ui;
    PEsquemaPage m_emptyPage; // empty page to load when no esquema is loaded
    QMap<QListWidgetItem*, PEsquemaPage*> m_itemPageMap;

    // NEW BOOKMARK - ESQ TREE VIEW
    // QStandardItemModel * m_esqModel;
};

#endif // PMAINESQUEMAUI_H
