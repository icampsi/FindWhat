// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <fstream>

#include "document/CMDoc.h"
#include "document/CEsquemaDoc.h"
#include "dialogs/exportEsquema_dlg.h"

// CONSTRUCTORS AND DESTRUCTORS ----------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), newEsquemadlg{nullptr}, ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setWindowIcon(QIcon(":/logo/mainLogo.ico")); // Loads the .ico logo for the application

    // SetUp the Dock Prveiew Widget
    m_dockPreview = new PDockPreview(this);
    m_dockPreview->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_dockPreview);


    // Check if there is any loaded esquema to export
    if (CMDoc::getMDoc().getLoadedEsquemaDocs()->size() == 0) {
        QMenu *fileMenu = ui->menuFile;

        // Find the "Export Esquema" action
        foreach(QAction *action, fileMenu->actions()) {
            if(action->text() == "Export Esquema") {
                m_exportEsquemaAction = action;
                m_exportEsquemaAction->setEnabled(false); // Disable the action
                break;
            }
        }
    }

    // CONNECTIONS
    // Menú actions connections
    connect(ui->action_NewEsquema   , &QAction::triggered, this, &MainWindow::action_newEsquema);
    connect(ui->action_LoadSession  , &QAction::triggered, this, &MainWindow::action_loadSession);
    connect(ui->action_SaveSession  , &QAction::triggered, this, &MainWindow::action_saveSession);
    connect(ui->action_ImportEsquema, &QAction::triggered, this, &MainWindow::action_importEsquema);
    connect(ui->action_ExportEsquema, &QAction::triggered, this, &MainWindow::action_exportEsquema);

    // Mediate between browserWidget and dockPreview
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, m_dockPreview    , &PDockPreview::handleFilePathChanged);
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, ui->mainEsquemaUI, &PMainEsquemaUI::handleFilePathChanged);

    // Enable sorting by column
    ui->browserWidget->setSortingEnabled(true);
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_dockPreview;
}

// PUBLIC FUNCTIONS ----------------------------------------------------
void MainWindow::loadEsquema(CEsquemaDoc* esquemaDoc) {
    ui->mainEsquemaUI->newEsquema(esquemaDoc);
}

void MainWindow::functionUpdated(CFormula::IndexPosition index, QString result) {
    m_dockPreview->handleFunctionUpdated(index, result);
}

// MENU ACTIONS --------------------------------------------------------
void MainWindow::action_newEsquema() {
    std::vector<QString> esquemaNames;
    for(auto it : *CMDoc::getMDoc().getLoadedEsquemaDocs()) {
        esquemaNames.push_back(it->getEsquema()->getName());
    }
    newEsquemadlg = new newEsquema_dlg(this, esquemaNames);
    newEsquemadlg->exec();

    checkExortEsquemaActionEnable();
}

void MainWindow::action_loadSession() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", QDir::homePath(), "FindWhat Session Files (*.fw)");
    if (fileName.isEmpty()) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Unsaved Changes",
                                  "Loading a new session will replace current one and discard unsaved changes. Continue?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    for (int i{0}; i < CMDoc::getMDoc().getLoadedEsquemaDocs()->size(); i++) {
        ui->mainEsquemaUI->handleDeleteEsquema(i);
    }

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (file.is_open()) {
        std::vector<CEsquemaDoc*> loadedEsquemaDocs;
        CMDoc::getMDoc().deserialize(file, loadedEsquemaDocs);
        file.close();
        for (CEsquemaDoc* esquemaDoc : loadedEsquemaDocs) {
            loadEsquema(esquemaDoc);
        }

    } else {
        QString errorString = "Couldn't open file " + fileName;
        QMessageBox::critical(this, "Error", errorString);
    }
}

void MainWindow::action_saveSession() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save .bin File", QDir::homePath(), "FindWhat Session Files (*.fw)");
    std::ofstream file(fileName.toStdString(), std::ios::binary);
    if (file.is_open()) {
        CMDoc::getMDoc().serializeFullEsquemaArray(file);
        file.close();
    } else {
        QString errorString = "Couldn't save file " + fileName;
        QMessageBox::critical(this, "Error", errorString);
    }
}

void MainWindow::action_importEsquema() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", QDir::homePath(), "FindWhat Esquema Files (*.esq)");
    if (fileName.isEmpty()) return;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (file.is_open()) {
        std::vector<CEsquemaDoc*> loadedEsquemaDocs;
        CMDoc::getMDoc().deserialize(file, loadedEsquemaDocs);
        file.close();
        for (CEsquemaDoc* esquemaDoc : loadedEsquemaDocs) {
            loadEsquema(esquemaDoc);
        }

    } else {
        QString errorString = "Couldn't open file " + fileName;
        QMessageBox::critical(this, "Error", errorString);
    }
}

void MainWindow::action_exportEsquema() {
    // Check if there is any loaded esquema to export
    if (CMDoc::getMDoc().getLoadedEsquemaDocs()->size() == 0) {
        QMessageBox::information(this, "Empty", "There are no esquemes to export");
        return;
    }

    size_t esquemaIndex{0}; // For storing index selected on the dialog combo box
    // Creates a dialog for selecting esquema to export
    exportEsquemaDlg = new exportEsquema_dlg(this, &esquemaIndex);
    int result = exportEsquemaDlg->exec();
    if (result == QDialog::Rejected) return;

    // Creates a dialog for choosing file name and path
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save .esq File", QDir::homePath(), "FindWhat Esquema Files (*.esq)");

    // Serialize ou the selected esquema
    std::ofstream file(fileName.toStdString(), std::ios::binary);
    if (file.is_open()) {
        CMDoc::getMDoc().serializeEsquema(file, CMDoc::getMDoc().getLoadedEsquemaDocs()->at(esquemaIndex));
        file.close();
    } else {
        QString errorString = "Couldn't save file " + fileName;
        QMessageBox::critical(this, "Error", errorString);
    }
}

void MainWindow::on_btn_changeRoot_clicked() {
    QString newRootPath = QFileDialog::getExistingDirectory(this, tr("Select Root Directory"), QDir::homePath());
    if (!newRootPath.isEmpty()) {
        QFileSystemModel* model = qobject_cast<QFileSystemModel*>(ui->browserWidget->model());
        if (model) {
            model->setRootPath(newRootPath);
            // Update the root index of the tree view
            ui->browserWidget->setRootIndex(model->index(newRootPath));
        }
    }
}

void MainWindow::on_lineEdit_rowFormat_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    CEsquemaDoc *esquemaDoc     = CMDoc::getMDoc().getActiveEsquemaDoc();
    CEsquema    *currentEsquema = nullptr;

    if (esquemaDoc)     currentEsquema = esquemaDoc->getEsquema();
    // if (currentEsquema) currentEsquema->constructCsvFormatFormulaStruct(ui->lineEdit_rowFormat->text(), '\"', ','); //set m_formatedFormula
}

void MainWindow::checkExortEsquemaActionEnable() {
    if (CMDoc::getMDoc().getLoadedEsquemaDocs()->size() == 0)  {
        m_exportEsquemaAction->setEnabled(false);
    }
    else {
        m_exportEsquemaAction->setEnabled(true);
    }
}
