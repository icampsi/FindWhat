/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PMainEsquemaUI.h"

#include "MainWindow.h"
#include "ui/dialogs/DDisplayParsedInfo.h"
#include "ui/dialogs/ProgBarExport_dlg.h"
#include "PFormExpToolBoxPage.h"
#include "ui_PMainEsquemaUI.h"

#include <QToolBox>
#include <QtPreprocessorSupport>
#include <QFileDialog>

#include "PEsquemaPage.h"
#include "PFormExpToolBoxPage.h"
#include "WExtendedListWidget.h"

#include "document/CMDoc.h"
#include "document/CExportPathDoc.h"

#include "utils/USystem.h"

#ifdef ENABLE_DBMANAGER
#include "dbManager/WDbView.h"
#endif

PMainEsquemaUI::PMainEsquemaUI(QWidget *parent)
    : QWidget(parent), ui(new Ui::PMainEsquemaUI), m_emptyPage(this)
{
    ui->setupUi(this);

    // TOOL BAR SETUP
    connect(ui->toolBar, &WToolBarEsquema::optionChanged, this, &PMainEsquemaUI::esquemaOptionChanged);

    // ESQUEMA LIST SETUP
    {
        WExtendedListWidget* esqList = ui->esqList;

        connect(esqList, &WExtendedListWidget::itemDeletitionRequested, this, &PMainEsquemaUI::handleDeleteEsquema);
        connect(esqList, &QListWidget::itemSelectionChanged           , this, &PMainEsquemaUI::handleEsquemaSelectionChanged);

        connect(esqList->model(), &QAbstractItemModel::rowsMoved, this, &PMainEsquemaUI::handleEsqItemsMoved);
    }

    // ESQUEMA PAGE SETUP
    {
        ui->stackedWidget_esquemaPage->addWidget(&m_emptyPage);
        setCurrentPageToEmptyPage();
        ui->stackedWidget_esquemaUI->setCurrentIndex(0);
    }

    // TAB WIDGET SETUP
    {
        ExtTabWidget* wtab = ui->toolBox_formatEsquema;

        // Disable "export CSV" button if no toolbox pages are loaded
        if(ui->toolBox_formatEsquema->count() == 0) {
            ui->pushButton_parse->setEnabled(false);
        }

        wtab->setTabsClosable(true);
        connect(wtab, &QTabWidget::tabCloseRequested, this, &PMainEsquemaUI::deletePage);
        connect(static_cast<ExtTabBar*>(wtab->tabBar()), &ExtTabBar::addTabClicked, this, &PMainEsquemaUI::addPage);
    }

    // PARSER SETUP
    {
        ui->radioButton_csvParse->setChecked(true);
    }

    // DBMANAGER SETUP
#ifdef ENABLE_DBMANAGER
    {
        WDbView *dbView = new WDbView(ui->stackedWidget_esquemaUI);
        ui->stackedWidget_esquemaUI->addWidget(dbView);
    }
#endif
    // NEW BOOKMRAK  - ESQ TREE VIEW
    // m_esqModel = new QStandardItemModel();
    // ui->esqView->setModel(m_esqModel);
}

PMainEsquemaUI::~PMainEsquemaUI() { delete ui; }

void PMainEsquemaUI::newEsquema(CEsquema* esquema) {
    PEsquemaPage *newPage = new PEsquemaPage(esquema, this);
    ui->stackedWidget_esquemaPage->addWidget(newPage);
    ui->stackedWidget_esquemaPage->setCurrentWidget(newPage);

    QListWidgetItem *esquemaItem = new QListWidgetItem(ui->esqList);
    esquemaItem->setFlags(esquemaItem->flags() | Qt::ItemIsEditable);
    esquemaItem->setText(esquema->getName());
    m_itemPageMap.insert(esquemaItem, newPage);

    // Add the item to the QListWidget and select it
    ui->esqList->addItem(esquemaItem);
    ui->esqList->setCurrentItem(esquemaItem);

    // NEW BOOKMARK - ESQ TREE VIEW
    // QStandardItem* esquemaItem2 = new QStandardItem(esquema->getName());
    // esquemaItem2->setData(QVariant::fromValue(esquema));  // Store pointer to CEsquema

    // for (const CFormula* formula : esquema->getExtractDataFormula()) {
    //     QStandardItem* formulaItem = new QStandardItem(formula->getDataName());
    //     formulaItem->setData(QVariant::fromValue(formula));  // Store pointer to CFormula
    //     esquemaItem2->appendRow(formulaItem);
    // }

    // m_esqModel->appendRow(esquemaItem2);


    // // Retrieving pointers
    // QStandardItem* topLevelItem = m_esqModel->item(1, 0);
    // if (topLevelItem) {
    //     CEsquema* retrievedEsq = topLevelItem->data().value<CEsquema*>();
    //     qDebug() << "Retrieved Esquema pointer:" << retrievedEsq;
    //     qDebug() << "Loaded Esquema name: " << retrievedEsq->getName();

    //     QStandardItem* childItem = topLevelItem->child(3, 3);
    //     if (childItem) {
    //         CFormula* retrievedFormula = childItem->data().value<CFormula*>();
    //         qDebug() << "Retrieved Formula pointer:" << retrievedFormula;
    //         qDebug() << "Loaded Formula name: " << retrievedFormula->getDataName();
    //     } else {
    //         qDebug() << "No child item found at index" << 0;
    //     }
    // } else {
    //     qDebug() << "No top-level item found at index" << 0;
    // }
}

void PMainEsquemaUI::handleEsquemaSelectionChanged() {
    // Check if any esquema is loaded in the ui
    if(!ui->esqList->selectedItems().isEmpty()) {
        QListWidgetItem *item = ui->esqList->selectedItems().front();
        if(item) {
            changeCurrentPage(m_itemPageMap[item]);
        } else qDebug() << "No item selected in list_esquemes";
    }
    else { // Else, set the esquema page to m_emptyPage of PMainEsquemaUI
        setCurrentPageToEmptyPage();
    }
}

void PMainEsquemaUI::handleEsqItemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {
    Q_UNUSED(parent);
    Q_UNUSED(end);
    Q_UNUSED(destination);

    if(start < row) row--;
    CMDoc::getMDoc().reorderEsqDocs(start, row);
}

void PMainEsquemaUI::addExportCSV(CExportCSV *exportCSV) {
    // Create a new page and bind it to the exportCSV
    PFormExpToolBoxPage *newToolBoxPage = new PFormExpToolBoxPage(ui->toolBox_formatEsquema, exportCSV);

    // Set a meaningful name or label for the page (adjust as needed)
    QString pageName = QString("New Page");
    ui->toolBox_formatEsquema->addTab(newToolBoxPage, pageName);

    // Connect the radio buttons to the stackedbox index of the PFormExpToolBoxPage.
    // BOOKMARK - This is a stupid way to do that and works only because for now i have 2 options, so sending the state of one
    // radiobutton is enough to get the correct page to display. Should change it at some point.
    connect(ui->radioButton_dbParse, &QRadioButton::toggled, newToolBoxPage, &PFormExpToolBoxPage::changePageOpt);
    newToolBoxPage->changePageOpt(ui->radioButton_dbParse->isChecked()); // setup current option for new pages
    // Ensure the new page is visible
    ui->toolBox_formatEsquema->setCurrentWidget(newToolBoxPage);

    // Enable UI elements based on the number of pages loaded
    int pageCount = ui->toolBox_formatEsquema->count();
    ui->pushButton_parse->setEnabled(pageCount > 0);
}

void PMainEsquemaUI::deletePage(int index) {
    // Check if the index is valid
    if (index < 0 || index >= ui->toolBox_formatEsquema->count()) {
        qDebug() << "Invalid index:" << index;
        return;
    }

    // Remove the associated document
    CMDoc::getMDoc().getExportPathDoc().deleteExportCSV(index);

    // Remove the page from the toolBox
    QWidget *pageWidget = ui->toolBox_formatEsquema->widget(index);
    ui->toolBox_formatEsquema->removeTab(index);
    if (pageWidget) {
        delete pageWidget; // This deletes the object associated with the page
    }
    // Get the count after removing the item
    int count = ui->toolBox_formatEsquema->count();

    // Select the last page if there are pages left
    if (count > 0) {
        ui->toolBox_formatEsquema->setCurrentIndex(count - 1);
    } else {
        // Update the state of parse button based on the number of pages left
        ui->pushButton_parse->setEnabled(false);
    }
}


void PMainEsquemaUI::clearPages() {
    int pages = ui->toolBox_formatEsquema->count();
    for(int i{pages - 1}; i >= 0; i--) {
        deletePage(i);
    }
}

void PMainEsquemaUI::on_pushButton_parse_clicked() {
    CMDoc& cmdoc = CMDoc::getMDoc();
    if (cmdoc.getLoadedEsquemaDocs()->size() == 0) {
        QMessageBox::information(this, "Empty esquema list", "Need to define at least one esquema to extract data from PDF files");
        return;
    }

    ui->pushButton_parse->setEnabled(false); // Disable button to avoid conflicts while parsing

    CExportPathDoc& exportPathDoc = CMDoc::getMDoc().getExportPathDoc();
    // Get all the loaded exportCSV as a vector
    const std::vector<CExportCSV*>& exportCSVs = exportPathDoc.getExportCSVs();

    // Checks the ammount of work that will be needed (if any) to set up the progress bar dialog
    const size_t fileCount = exportPathDoc.getFileCount();
    QStandardItemModel* combinedModel = new QStandardItemModel();
    // Creates progressBar dialog
    if(fileCount > 0) {
        ProgBarExport_dlg *progressDlg = new ProgBarExport_dlg(fileCount, this);
        progressDlg->show();
        QCoreApplication::processEvents(); // Needed to display progress bar

        // Counts the max ammount of columns there are in any format table
        size_t maxColumns{ 1 }; // 1 if dbParser is checked. Iterate through exportCSVs to count otherwise
        if(ui->radioButton_csvParse->isChecked()) {
            for (CExportCSV* it : exportCSVs) {
                QAbstractItemModel *model = it->getCsvTableModel();
                if(!model) continue;
                size_t columnCount = model->columnCount();
                if(columnCount > maxColumns) maxColumns = columnCount;
            }
        }

        std::vector<std::vector<QString>> newData;
        for (CExportCSV* it : exportCSVs) {
            newData.clear();
            // DOING THE ACTUAL WORK: Build the structure
            it->buildStructure(combinedModel, progressDlg, maxColumns, ui->radioButton_dbParse->isChecked());
        }

        // Delete progress dialog
        delete progressDlg;
        progressDlg = nullptr;

        // Display Table DIalog
        DDisplayParsedInfo *piDialog = new DDisplayParsedInfo(combinedModel, this);
        piDialog->exec();
    }
    if(combinedModel) delete combinedModel;
    ui->pushButton_parse->setEnabled(true);
}

void PMainEsquemaUI::handleFilePathChanged(const QString &filePath) {
    Q_UNUSED(filePath);

    QWidget *currentWidget   = ui->stackedWidget_esquemaPage->currentWidget();
    PEsquemaPage *esquemaPage = qobject_cast<PEsquemaPage*>(currentWidget);

    if (esquemaPage) {
        esquemaPage->updateFunctionProcess();
    } else {
        qDebug() << "Current widget is not an instance of EsquemaPage";
    }
}

void PMainEsquemaUI::handleDeleteEsquema(const int index) {
    deleteEsquema(index, true);
}

void PMainEsquemaUI::deleteEsquema(const int index, const bool askConfirmation) {
    QListWidgetItem* item = ui->esqList->item(index);
    if (!item) {
        // Handle the case where the item does not exist
        return;
    }

    if (askConfirmation) {
        // Show confirmation dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirm Deletion",
                                      QString("Are you sure you want to delete this esquema: '%1'?").arg(item->text()),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    // Find the associated PEsquemaPage and delete it
    if (m_itemPageMap.contains(item)) {
        PEsquemaPage* page = m_itemPageMap.value(item);
        delete page;  // Delete the associated PEsquemaPage
        m_itemPageMap.remove(item);  // Remove the entry from the map
    }

    // Delete the item from the QListWidget
    delete ui->esqList->takeItem(index);

    // Delete esquema from the document
    CMDoc::getMDoc().deleteEsquema(index);

    // Perform a check in main window for disabling Export Esquema menu action if needed
    qobject_cast<MainWindow*>(SystemUtils::getLastParent(this))->checkExortEsquemaActionEnable();
}

