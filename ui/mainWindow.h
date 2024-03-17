/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "PDockPreview.h"

#include "dialogs/NewEsquema_dlg.h"
#include "dialogs/exportEsquema_dlg.h"

#include "document/CEsquemaDoc.h"

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
    PDockPreview      *m_dockPreview = nullptr;
    newEsquema_dlg    *newEsquemadlg = nullptr;
    exportEsquema_dlg *exportEsquemaDlg = nullptr;


private slots:
    // Menú actions
    void action_newEsquema();
    void action_loadSession();
    void action_saveSession();
    void action_importEsquema();
    void action_exportEsquema();

    // Widget slots
    void on_btn_changeRoot_clicked();

public slots:
    void functionUpdated(CFormula::IndexPosition index, QString result);

public:
    void loadEsquema(CEsquemaDoc* esquema);
    void checkExortEsquemaActionEnable();

private:
    Ui::MainWindow *ui;
    QAction* m_exportEsquemaAction = nullptr; // Pointer to export esquema menu action for easy validate or invalidate depending on esquemes being loaded
};

#endif // MMAINWINDOW_H
