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

    // Mediate between browserWidget and dockPreview
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, m_dockPreview    , &PDockPreview::handleFilePathChanged);
    connect(ui->browserWidget, &WBrowserTreeView::filePathChanged, ui->mainEsquemaUI, &PMainEsquemaUI::handleFilePathChanged);

    // Menú actions connections
    connect(ui->action_NewEsquema , &QAction::triggered, this, &mainWindow::action_newEsquema);
    connect(ui->action_LoadEsquema, &QAction::triggered, this, &mainWindow::action_loadEsquema);
    connect(ui->action_SaveEsquema, &QAction::triggered, this, &mainWindow::action_saveEsquema);


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
    QMessageBox::information(this, "Under Construction", "This action won't work until serialization is implemented");
}

void mainWindow::action_saveEsquema() {
    QMessageBox::information(this, "Under Construction", "This action won't work until serialization is implemented");
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
    if (currentEsquema) currentEsquema->setCsvFormatFormula(ui->lineEdit_rowFormat->text(), '\"', ','); //set m_formatedFormula
}


void mainWindow::on_pushButton1_clicked()
{
    QString fileName = "function.bin";
    std::ofstream file(fileName.toStdString(), std::ios::binary);
    CExtractingFunction *function = new CExtractingFunction(CFunction::FunctionType::Find);

    if (file.is_open()) {
        // file.write(data.data(), data.size());
        function->setFunctionTypeName("hola");
        function->serialize(file);
        // CFunction *desFunction = new CFunction(CFunction::FunctionType::ExtractData);
        // desFunction->deserialize();

        file.close();
        // std::cout << "Data saved to file: " << filename << std::endl;
    } else {
        qDebug() << "Error: Unable to open file for writing: " << "hola";
    }

    std::ifstream openFile(fileName.toStdString(), std::ios::binary);
    CExtractingFunction* desFuntion = new CExtractingFunction(CFunction::FunctionType::ExtractData);
    desFuntion->setCharTypeToGet(CExtractingFunction::CharTypeToGet::digit);
    desFuntion->setCharsToGet(333);
    desFuntion->setCharsToRead(333);
    desFuntion->setFunctionTypeName("333");
    desFuntion->setEndingString("333");
    desFuntion->setInvertedDirection(true);
    desFuntion->setToAllow("333");
    desFuntion->setToAvoid("333");

    if (openFile.is_open()) {
        desFuntion->deserialize(openFile);
    }
    file.close();
    if(function == desFuntion) {
        qDebug() << "yeeaaahhhhh";
    }
}

