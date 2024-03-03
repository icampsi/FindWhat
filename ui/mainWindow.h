#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "PDockPreview.h"

#include "dialogs/NewEsquema_dlg.h"
#include "document/CDocumentSubclasses.h"
#include "ui/dialogs/exportEsquema_dlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainWindow; }
QT_END_NAMESPACE

class mainWindow : public QMainWindow {
    Q_OBJECT

public:
    //Constructors and destructors
    mainWindow(QWidget *parent = nullptr);
    ~mainWindow();

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
    void on_lineEdit_rowFormat_textChanged(const QString &arg1);

public slots:
    void functionUpdated(CFormula::IndexPosition index, QString result);

public:
    void loadEsquema(CEsquemaDoc* esquema);
    void checkExortEsquemaActionEnable();

private:
    Ui::mainWindow *ui;
    QAction* m_exportEsquemaAction = nullptr; // Pointer to export esquema menu action for easy validate or invalidate depending on esquemes being loaded
};

#endif // MAINWINDOW_H
