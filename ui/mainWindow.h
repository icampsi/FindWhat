#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "PDockPreview.h"

#include "dialogs/NewEsquema_dlg.h"
#include "document/CDocumentSubclasses.h"

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
    PDockPreview   *m_dockPreview = nullptr;
    newEsquema_dlg *newEsquemadlg = nullptr;

private slots:
    // Menú actions
    void action_newEsquema();
    void action_loadEsquema();
    void action_saveEsquema();

    // Widget slots
    void on_btn_changeRoot_clicked();
    void on_lineEdit_rowFormat_textChanged(const QString &arg1);

    void on_pushButton_clicked();

public slots:
    void functionUpdated(CFormula::IndexPosition index, QString result);

public:
    void loadEsquema(CEsquemaDoc* esquema);

private:
    Ui::mainWindow *ui;
};

#endif // MAINWINDOW_H
