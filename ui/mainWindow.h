/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

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
    void inline action_showPreviewPanel();
    void inline action_showFileBrowserPanel();

    // Widget slots
    void on_btn_changeRoot_clicked();

public slots:
    void functionUpdated(CFormula::IndexPosition index, const QString& result) {
        m_dockPreview->handleFunctionUpdated(index, result);
    }

    void inline checkExortEsquemaActionEnable();

public:
    void inline loadEsquema(CEsquemaDoc* esquema);
};

#endif // MMAINWINDOW_H
