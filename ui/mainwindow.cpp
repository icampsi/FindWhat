#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <fstream>

#include "document/CMDoc.h"

// CONSTRUCTORS AND DESTRUCTORS ----------------------------------------
mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent), newEsquemadlg{nullptr}, ui(new Ui::mainWindow) {

    ui->setupUi(this);
    setWindowIcon(QIcon(":/logo/mainLogo.ico")); // Loads the .ico logo for the application

    // SetUp the Dock Prveiew Widget
    m_dockPreview = new PDockPreview(this);
    m_dockPreview->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_dockPreview);

    // CONNECTIONS
    // Menú actions connections
    connect(ui->action_NewEsquema , &QAction::triggered, this, &mainWindow::action_newEsquema);
    connect(ui->action_LoadEsquema, &QAction::triggered, this, &mainWindow::action_loadEsquema);
    connect(ui->action_SaveEsquema, &QAction::triggered, this, &mainWindow::action_saveEsquema);

    // Mediate between browserWidget and dockPreview
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, m_dockPreview    , &PDockPreview::handleFilePathChanged);
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, ui->mainEsquemaUI, &PMainEsquemaUI::handleFilePathChanged);

    // Enable sorting by column
    ui->browserWidget->setSortingEnabled(true);
}

mainWindow::~mainWindow() {
    delete ui;
    delete m_dockPreview;
}

// PUBLIC FUNCTIONS ----------------------------------------------------
void mainWindow::loadEsquema(CEsquemaDoc* esquemaDoc) {
    ui->mainEsquemaUI->newEsquema(esquemaDoc);
}

void mainWindow::functionUpdated(CFormula::IndexPosition index, QString result) {
    m_dockPreview->handleFunctionUpdated(index, result);
}

// MENU ACTIONS --------------------------------------------------------
void mainWindow::action_newEsquema() {
    std::vector<QString> esquemaNames;
    for(auto it : *CMDoc::getMDoc().getLoadedEsquemaDocs()) {
        esquemaNames.push_back(it->getEsquema()->getName());
    }
    newEsquemadlg = new newEsquema_dlg(this, esquemaNames);
    newEsquemadlg->show();
}

void mainWindow::action_loadEsquema() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", QDir::homePath(), "Binary Files (*.esq)");

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

void mainWindow::action_saveEsquema() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save .bin File", QDir::homePath(), "Binary Files (*.esq)");
    std::ofstream file(fileName.toStdString(), std::ios::binary);
    if (file.is_open()) {
        CMDoc::getMDoc().serialize(file);
        file.close();
    } else {
        QString errorString = "Couldn't save file " + fileName;
        QMessageBox::critical(this, "Error", errorString);
    }
}

void mainWindow::on_btn_changeRoot_clicked() {
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

void mainWindow::on_lineEdit_rowFormat_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    CEsquemaDoc *esquemaDoc     = CMDoc::getMDoc().getActiveEsquemaDoc();
    CEsquema    *currentEsquema = nullptr;

    if (esquemaDoc)     currentEsquema = esquemaDoc->getEsquema();
    // if (currentEsquema) currentEsquema->constructCsvFormatFormulaStruct(ui->lineEdit_rowFormat->text(), '\"', ','); //set m_formatedFormula
}

