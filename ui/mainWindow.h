/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include "ui/ui_MainWindow.h"

#include <QMainWindow>
#include <QString>

#include "PDockPreview.h"

class CMDoc;
class CEsquemaDoc;
class newEsquema_dlg;
class exportEsquema_dlg;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    //Constructors and destructors
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Members
    PDockPreview      *m_dockPreview    = nullptr;
    newEsquema_dlg    *newEsquemadlg    = nullptr;
    exportEsquema_dlg *exportEsquemaDlg = nullptr;
    CMDoc& m_cmdoc;

private:
    Ui::MainWindow *ui;

    QAction* m_exportEsquemaAction; // Pointer to export esquema menu action for easy validate or invalidate depending on esquemes being loaded
    bool     showFormatingMarks;    // Flag to show/hide formating marks

private slots:
    // Menú actions
    void action_newEsquema();
    void action_loadSession();
    void action_saveSession();
    void action_importEsquema();
    void action_exportEsquema();

    void action_showPreviewPanel() {
        if(m_dockPreview->isHidden()) m_dockPreview->show();
        else                          m_dockPreview->hide();
    }

    void action_showFileBrowserPanel() {
        if(ui->dockWidget_fileBrowser->isHidden()) ui->dockWidget_fileBrowser->show();
        else                                       ui->dockWidget_fileBrowser->hide();
    }

    // Widget slots
    void on_btn_changeRoot_clicked();

public slots:
    void functionUpdated(CFormula::IndexPosition index, const QString& result) { m_dockPreview->handleFunctionUpdated(index, result); }
    void checkExortEsquemaActionEnable();

public:
    void loadEsquema(CEsquemaDoc* esquemaDoc) { ui->mainEsquemaUI->newEsquema(esquemaDoc); }};

#endif // MMAINWINDOW_H
