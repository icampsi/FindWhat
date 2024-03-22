/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PEsquemaPage.h"
#include "ui_PEsquemaPage.h"

#include <QMenu>

#include "MainWindow.h"

#include "utils/GeneralFunctions.h"

#include "src/CData.h"

#include "document/CMDoc.h"
#include "document/CPdfDoc.h"
#include "document/CEsquemaDoc.h"

// From generalfunctions.h
QString parseFromText(const QString& text);
QString parseToText(const QString& text);

// CONSTRUCTORS AND DESTRUCTORS ----------------------------------------
PEsquemaPage::PEsquemaPage(CEsquemaDoc* esquemaDoc, QWidget *parent)
    : QWidget(parent), ui(new Ui::PEsquemaPage), m_esquemaDoc {esquemaDoc} {
    ui->setupUi(this);

    model_esquema = new QStandardItemModel();
    ui->stacked_editFunction->setCurrentIndex(0);
    ui->stackedWidget_general->setCurrentIndex(0);

    ui->treeView_Esquema->setModel(model_esquema);
    connect(ui->treeView_Esquema, &WEsquemaTreeView::itemEditingFinished, this, &PEsquemaPage::handleItemEditFinish);

    loadEsquema();
    ui->treeView_Esquema->expandAll();

    connect(ui->treeView_Esquema, &WEsquemaTreeView::removeSecondLevel, this, &PEsquemaPage::handleRemoveSecondLevel);
    connect(this, &PEsquemaPage::functionUpdated, static_cast<MainWindow*>(getLastParent(this)), &MainWindow::functionUpdated);
    connect(ui->listWidget_formula->model(), &QAbstractItemModel::rowsMoved, this, &PEsquemaPage::handleFunctionItemsMoved);
    connect(ui->endingStringBlock, &PEndingStringBlock::functionUpdated, this, &PEsquemaPage::handleFunctionUpdated);
}

PEsquemaPage::PEsquemaPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::PEsquemaPage), m_esquemaDoc {nullptr} {
    ui->setupUi(this);
    ui->frame_esquema->setEnabled(false);
    ui->stackedWidget_general->setCurrentIndex(0);
}

PEsquemaPage::~PEsquemaPage() { delete ui; }

// PUBLIC FUNCTIONS ----------------------------------------------------
void PEsquemaPage::handleRemoveSecondLevel(const int index, const QModelIndex &parentIndex) {
    QStandardItemModel *itemModel = dynamic_cast<QStandardItemModel*>(ui->treeView_Esquema->model());
    QStandardItem *item = nullptr;

    if(itemModel) {
        item = itemModel->itemFromIndex(ui->treeView_Esquema->model()->index(index, 0, parentIndex));

        // HANDLE REMOVE STATICDATA
        if(parentIndex.row() == 0) {
            m_itemDataMap.remove(item);
            m_esquemaDoc->getEsquema()->deleteStaticData(index);
            ui->treeView_Esquema->model()->removeRow(index, parentIndex);
        }

        // HANDLE REMOVE FORMULA
        if(parentIndex.row() == 1) {
            m_itemFormulaMap.remove(item);
            m_esquemaDoc->getEsquema()->deleteFormula(index);
            ui->treeView_Esquema->model()->removeRow(index, parentIndex);
        }

        ui->treeView_Esquema->clearSelection();
        ui->stackedWidget_general->setCurrentIndex(0);
    }
}

void PEsquemaPage::handleFunctionUpdated() {
    updateFunctionProcess();
}

void PEsquemaPage::loadFunction() {
    // Retrieve the selected item
    QListWidgetItem *item = ui->listWidget_formula->currentItem();
    if(!item) {
        ui->stacked_editFunction->setCurrentIndex(0);
        return;
    }

    CFunction *function = m_itemFunctionMap.value(item);
    // Map to function
    if(!function) {
        QMessageBox::information(this, "Information", "Function not found for the selected item.");
        return;
    }
    m_activeFunction = function;
    ui->stacked_editFunction->setCurrentIndex(static_cast<int>(function->getFunctionType()) + 1);

    updateFunctionProcess();

    CIndexingFunction   *indexingFunction   = dynamic_cast<CIndexingFunction*>(function);
    CExtractingFunction *extractingFunction = dynamic_cast<CExtractingFunction*>(function);
    // CMathFunction       *mathFunction       = dynamic_cast<CMathFunction*>(function); // Still unused until futur updates
    QString parsedText;
    switch (function->getFunctionType()) {
    case CFunction::Action::Find:
        parsedText = parseToText(indexingFunction->getText());
        if(ui->lineEdit_textToFind->text() != parsedText) ui->lineEdit_textToFind->setText(parsedText);
        if(ui->comboBox_setIndexAt->currentIndex() != !indexingFunction->getOption())ui->comboBox_setIndexAt->setCurrentIndex(!indexingFunction->getOption());
        ui->comboBox_startFrom->setCurrentIndex(indexingFunction->getStartFromBeggining());
        break;
    case CFunction::Action::MoveIndex:
        break;
    case CFunction::Action::MoveLine:
        break;
    case CFunction::Action::AppendString:
        parsedText = parseToText(indexingFunction->getText());
        ui->lineEdit_stringToAppend->setText(parsedText);
        if(!indexingFunction->getOption()) ui->radioButton_append->setChecked(true);
        if(indexingFunction->getOption()) ui->radioButton_preppend->setChecked(true);
        break;
    case CFunction::Action::ExtractData:
        ui->comboBox_readDirection->setCurrentIndex(extractingFunction->isInverted());
        ui->comboBox_typeOfData->setCurrentIndex(static_cast<int>(extractingFunction->getCharTypeToGet()));
        ui->lineEdit_charsToAllow->setText(extractingFunction->getToAllow());
        ui->lineEdit_charsToAvoid->setText(extractingFunction->getToAvoid());
        ui->endingStringBlock->updateBlock(static_cast<CExtractingFunction*>(m_activeFunction));

        break;
    }

    QString functionName = item->text();
    functionName.replace(function->getFunctionTypeName() + ": ", "");
    ui->lineEdit_functionName->setText(functionName);
}

void PEsquemaPage::updateFunctionProcess() {
    if (!m_loadedFormula | !m_activeFunction) return; // We won't be doing any function update if there is no function
    QListWidgetItem *item = ui->listWidget_formula->currentItem();

    // Applay formula up to this selected function
    int functionIndex;
    if(item) {
        functionIndex = ui->listWidget_formula->indexFromItem(item).row(); // Get function index
    }
    else functionIndex = -1; // if no item is provided the formula will go up to the end of the function path

    CPdfDoc *activePdfDoc = CMDoc::getMDoc().getActivePdfDoc();

    // If there is no active doc we don't need to update anything in the preview;
    if(activePdfDoc) {
        // Update extracted data and result text widgets
        CFormula::Result halfResult;
        CFormula::Result result = m_loadedFormula->applyFormula(activePdfDoc, 0, functionIndex, &halfResult);
        ui->plainTextEdit_resultToSelectedFunction->setPlainText(result.result);

        {
            size_t initialIndex = result.indexPosition.initial;
            size_t finalIndex   = result.indexPosition.final;
            emit functionUpdated(CFormula::IndexPosition{ initialIndex, finalIndex },  result.result); // BOOKMARK - Could change emit siognature to get onli CFormulaResult
        }

        CFunction* function = m_itemFunctionMap[item];
        if(function && function->getFunctionType() == CFunction::Action::ExtractData) {
            ui->textEdit_extractedData->setText(halfResult.result);
            ui->textEdit_finalResult->setText(result.result);
        }
    }
}

void PEsquemaPage::loadEsquema() {
    CEsquema* esquema = m_esquemaDoc->getEsquema();

    QStandardItem *staticDataParentItem = new QStandardItem("Static Data");
    QStandardItem *formulas = new QStandardItem("Formulas");
    QStandardItem *rootItem = model_esquema->invisibleRootItem();
    rootItem->appendRow(staticDataParentItem);
    rootItem->appendRow(formulas);
    // Create third level items from esquema.m_staticData[] attached to "Static Data"
    for(size_t i{0}; i < esquema->getStaticData().size(); i++) {
        QStandardItem *staticDataItem = new QStandardItem(esquema->getStaticData()[i]->getDataName()); // Create new item for each member of the vector
        CData *staticData = esquema->getStaticData()[i];
        staticDataItem->setData(QVariant::fromValue(staticData), Qt::UserRole);
        m_itemDataMap[staticDataItem] = staticData;
        staticDataParentItem->appendRow(staticDataItem);
    }

    // Create third level items from esquema.t_extractDataFormula[] attached to "Formulas"
    for(size_t i{0}; i < esquema->getExtractDataFormula().size(); i++) {
        QStandardItem *formulaItem = new QStandardItem(esquema->getExtractDataFormula()[i]->getDataName()); // Create new item for each member of the vector
        CFormula *formula = esquema->getExtractDataFormula()[i];
        formulaItem->setData(QVariant::fromValue(formula), Qt::UserRole);
        m_itemFormulaMap[formulaItem] = formula;
        formulas->appendRow(formulaItem);
    }
}

void PEsquemaPage::newFormula() {
    CFormula* newFormula = new CFormula("Unnamed formula");
    CEsquema* esquema = m_esquemaDoc->getEsquema();

    esquema->addExtractDataFormula(newFormula);

    // Create second level items from esquema.t_extractDataFormula attached to "Formulas"
    QStandardItem* formulaItem = new QStandardItem(newFormula->getDataName());
    formulaItem->setData(QVariant::fromValue(newFormula), Qt::UserRole);
    m_itemFormulaMap[formulaItem] = newFormula;
    model_esquema->item(1, 0)->appendRow(formulaItem);
}

void PEsquemaPage::newStaticData() {
    CData *newStaticData = new CData("Unnamed data", "");
    CEsquema *esquema    = m_esquemaDoc->getEsquema();

    esquema->addStaticData(newStaticData);

    // Create second level items from esquema.t_extractDataFormula attached to "Formulas"
    QStandardItem *staticDataItem = new QStandardItem(newStaticData->getDataName());
    staticDataItem->setData(QVariant::fromValue(newStaticData), Qt::UserRole);
    m_itemDataMap[staticDataItem] = newStaticData;
    model_esquema->item(0, 0)->appendRow(staticDataItem);
}

// SLOTS ---------------------------------------------------------------
// VIEWS
void PEsquemaPage::on_treeView_Esquema_clicked(const QModelIndex &index) {
    if (!index.isValid() || !index.parent().isValid()) return;

    ui->listWidget_formula->clear();
    m_loadedFormula = nullptr; // Just in case, so can't try to acces the previous loaded formula if there where any.
    ui->stackedWidget_general->setCurrentIndex(1);

    // STATIC DATA
    if(index.parent().row() == 0) {       
        QStandardItem *retrievedDataItem = model_esquema->itemFromIndex(index);
        // Load data member
        m_loadedStaticData = m_itemDataMap.value(retrievedDataItem);
        // Update label and text editor
        ui->label_dataName->setText(m_loadedStaticData->getDataName());
        // Update plainText editor
        ui->plainTextEdit_staticDataString->setPlainText(m_loadedStaticData->getDataString());
        // Set stacked widget page so it shows the data editor
        ui->stackedWidget_dataOrFormula->setCurrentIndex(0);
    }

    // FORMULA
    if(index.parent().row() == 1) {
        QStandardItem *retrievedFormulaItem = model_esquema->itemFromIndex(index);
        // Load formula member
        m_loadedFormula = m_itemFormulaMap.value(retrievedFormulaItem);
        // Update label
        ui->label_dataName->setText(m_loadedFormula->getDataName());

        QString functionName;
        // Add function items to listWidget_formula
        for(size_t i{0}; i < m_loadedFormula->getPathSize(); i++) {
            CFunction* function = m_loadedFormula->getFunction(static_cast<int>(i));
            functionName = function->getFunctionTypeName() + ": ";
            QListWidgetItem * functionItem = new QListWidgetItem(functionName);
            m_itemFunctionMap[functionItem] = function;
            ui->listWidget_formula->addItem(functionItem);
        }
        // If any function is loaded, show the first one, else, empty stacked edit function page by showing the empty page on index 0
        if(ui->listWidget_formula->count() > 0) {
            ui->listWidget_formula->setCurrentRow(0);
        }
        else {
            ui->stacked_editFunction->setCurrentIndex(0);
        }
        updateFunctionProcess();
        // Set stacked widget page so it shows the formula editor
        ui->stackedWidget_dataOrFormula->setCurrentIndex(1);
    }
}

void PEsquemaPage::handleFunctionItemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {
    Q_UNUSED(parent);
    Q_UNUSED(end);
    Q_UNUSED(destination);

    if(start < row) row--;
    m_loadedFormula->reorderFunctionPath(start, row);
}

void PEsquemaPage::handleItemEditFinish(const QModelIndex &index, const QString &text) {
    if(index.parent().row() == 0) { // Handle Static Data Name edition
        m_esquemaDoc->getEsquema()->setStaticDataName(m_loadedStaticData, text);
        ui->label_dataName->setText(text);
    }
    else if(index.parent().row() == 1)  { // Handle formula name edition
        m_esquemaDoc->getEsquema()->setFormulaName(m_loadedFormula, text);
        ui->label_dataName->setText(m_loadedFormula->getDataName());
    }
}
// MANAGE FUNCTIONS
void PEsquemaPage::on_btn_newFunction_clicked() {
    // Create a menu
    QMenu   *menu                = new QMenu(this);
    QAction *action_Find         = menu->addAction("Find");
    QAction *action_MoveIndex    = menu->addAction("Move Index");
    QAction *action_MoveLine     = menu->addAction("Move Line");
    QAction *action_appendString = menu->addAction("Append String");
    QAction *action_extractData  = menu->addAction("Extract Data");

    // Connect actions' triggered signal to slots
    connect(action_Find        , &QAction::triggered, this, [=]() { handle_newFunActions(CFunction::Action::Find); });
    connect(action_MoveIndex   , &QAction::triggered, this, [=]() { handle_newFunActions(CFunction::Action::MoveIndex); });
    connect(action_MoveLine    , &QAction::triggered, this, [=]() { handle_newFunActions(CFunction::Action::MoveLine); });
    connect(action_appendString, &QAction::triggered, this, [=]() { handle_newFunActions(CFunction::Action::AppendString); });
    connect(action_extractData , &QAction::triggered, this, [=]() { handle_newFunActions(CFunction::Action::ExtractData); });

    menu->exec(ui->btn_newFunction->mapToGlobal(QPoint(0, ui->btn_newFunction->height())));
}

void PEsquemaPage::handle_newFunActions(CFunction::Action functionType) {
    CFunction* newFunction = nullptr;
    QListWidgetItem* item  = nullptr;
    if(m_loadedFormula) {
        switch (functionType) {
        case (CFunction::Action::Find):
        case CFunction::Action::MoveIndex:
        case CFunction::Action::MoveLine:
        case CFunction::Action::AppendString:
            newFunction = new CIndexingFunction(functionType);
            break;
        case CFunction::Action::ExtractData:
            newFunction = new CExtractingFunction(functionType);
            ui->endingStringBlock->setParentFunction(static_cast<CExtractingFunction*>(newFunction)); // Pass the new function to PEndingStringBlock
            connect(ui->endingStringBlock, &PEndingStringBlock::functionUpdated, this, &PEsquemaPage::handleFunctionUpdated);
            break;
        }

        item = new QListWidgetItem(newFunction->getFunctionTypeName());
        m_loadedFormula->addFunction(newFunction);
        m_itemFunctionMap[item] = newFunction;
        ui->listWidget_formula->addItem(item);
    }
    else { qDebug() << "No formula is been loaded yet"; }

    if (ui->listWidget_formula != nullptr && ui->listWidget_formula->count() > 0) {
        int lastIndex = ui->listWidget_formula->count() - 1;

        ui->listWidget_formula->setCurrentRow(lastIndex);
    } else {
        QMessageBox* messageBox = new QMessageBox(QMessageBox::Warning, "Formula required", "You need to select a formula first", QMessageBox::Ok);
        messageBox->exec();
        delete messageBox; // Deleting the QMessageBox object
    }  
}

void PEsquemaPage::on_lineEdit_functionName_textChanged(const QString &arg1) {
    QListWidgetItem* item = ui->listWidget_formula->currentItem();
    CFunction* function = m_itemFunctionMap[item];
    QString newName = function->getFunctionTypeName() + ": " + arg1;
    item->setText(newName);
}

void PEsquemaPage::on_pushButton_deleteFunctin_clicked() {
    QListWidget *listWidget = ui->listWidget_formula;
    if(listWidget->count() == 0) return; // if there are no functions, return
    int rowToDelete = listWidget->currentRow(); // Get the index of the current row

    // Ensure a valid row is selected
    if (rowToDelete != -1) {
        // Remove the item from the list widget
        QListWidgetItem *item = listWidget->takeItem(rowToDelete);
        delete item; // Delete the item
        m_loadedFormula->deleteFunction(rowToDelete);
    } else {
        // Handle case where no row is selected
        QMessageBox::warning(this, "No Row Selected", "Please select a row to delete.");
    }
    if(listWidget->count() == 0) m_activeFunction = nullptr; // if last function is been deleted, point m_activeFunction to nullptr
    updateFunctionProcess();
}

// STACKED BOX UI
// FIND FUNCTION UI
void PEsquemaPage::on_lineEdit_textToFind_textChanged(const QString &arg1) {
    QString parsedText = parseFromText(arg1);
    dynamic_cast<CIndexingFunction*>(m_activeFunction)->setText(parsedText);
    updateFunctionProcess();
}

void PEsquemaPage::on_comboBox_setIndexAt_currentIndexChanged(int index) {
    dynamic_cast<CIndexingFunction*>(m_activeFunction)->setOption(!index);
    updateFunctionProcess();
}

void PEsquemaPage::on_comboBox_startFrom_currentIndexChanged(int index) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    if(!index) function->setStartFromBeggining(false);
    else       function->setStartFromBeggining(true);
    updateFunctionProcess();
}

// EXTRACTING FUNCTION UI
void PEsquemaPage::on_comboBox_readDirection_currentIndexChanged(int index) {
    QListWidgetItem *item = ui->listWidget_formula->currentItem();
    if(!item) {
        qDebug() << "No items selected";
        return;
    }
    // Map to function
    CFunction *function = m_itemFunctionMap.value(item);
    if(!function) {
        qDebug() << "Function not found for the selected item.";
        return;
    }

    static_cast<CExtractingFunction*>(function)->setIsInverted(index);
    // Applay formula up to this selected function
    updateFunctionProcess();
}

void PEsquemaPage::on_lineEdit_charsToAllow_textEdited(const QString &arg1) {
    CExtractingFunction* function = static_cast<CExtractingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setToAllow(parseFromText(arg1));
    updateFunctionProcess();
}

void PEsquemaPage::on_lineEdit_charsToAvoid_textEdited(const QString &arg1) {
    CExtractingFunction* function = static_cast<CExtractingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setToAvoid(parseFromText(arg1));
    updateFunctionProcess();
}

// MOVE LINES UI
void PEsquemaPage::on_spinBox_moveLinesNum_valueChanged(int arg1) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setNum(arg1);
    updateFunctionProcess();
}

void PEsquemaPage::on_comboBox_placeInLine_activated(int index) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setOption(index);
    updateFunctionProcess();
}

// APPEND STRING UI
void PEsquemaPage::on_lineEdit_stringToAppend_textChanged(const QString &arg1) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setText(arg1);
    updateFunctionProcess();
}

void PEsquemaPage::on_radioButton_preppend_clicked() {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setOption(true);
    updateFunctionProcess();
}

void PEsquemaPage::on_radioButton_append_clicked() {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setOption(false);
    updateFunctionProcess();
}

// MOVE INDEX UI
void PEsquemaPage::on_spinBox_moveIndex_valueChanged(int arg1) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setNum(arg1);
    updateFunctionProcess();
}

void PEsquemaPage::on_comboBox_typeOfData_currentIndexChanged(int index) {
    CExtractingFunction* function = static_cast<CExtractingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setCharTypeToGet(static_cast<CExtractingFunction::CharTypeToGet>(index));
    updateFunctionProcess();
}

void PEsquemaPage::on_plainTextEdit_staticDataString_textChanged() {
    QString text = ui->plainTextEdit_staticDataString->toPlainText();
    m_loadedStaticData->setDataString(text);
}

void PEsquemaPage::on_checkBox_lookOnlyAtPage_stateChanged(int arg1) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    if(arg1) {
        ui->spinBox_lookOnlyAtPage->setEnabled(true);
        function->setNum(ui->spinBox_lookOnlyAtPage->value());
    }
    else {
        ui->spinBox_lookOnlyAtPage->setEnabled(false);
        function->setNum(0);
    }
    updateFunctionProcess();
}


void PEsquemaPage::on_spinBox_lookOnlyAtPage_valueChanged(int arg1) {
    CIndexingFunction* function = static_cast<CIndexingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setNum(arg1);
    updateFunctionProcess();
}


void PEsquemaPage::on_lineEdit_toReplace_textChanged(const QString &arg1) {
    CExtractingFunction* function = static_cast<CExtractingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setToReplace(arg1);
    updateFunctionProcess();
}


void PEsquemaPage::on_lineEdit_replaceFor_textChanged(const QString &arg1) {
    CExtractingFunction* function = static_cast<CExtractingFunction*>(m_itemFunctionMap[ui->listWidget_formula->currentItem()]);
    function->setReplaceFor(arg1);
    updateFunctionProcess();
}

