#ifndef PESQUEMAPAGE_H
#define PESQUEMAPAGE_H

#include <QWidget>

#include "qfilesystemmodel.h"
#include "qlistwidget.h"
#include "qstandarditemmodel.h"

#include "src/CEsquema.h"
#include "src/CFormula.h"
#include "dialogs/NewEsquema_dlg.h"
#include "document/CDocumentSubclasses.h"

namespace Ui { class esquemaPage; }

class EsquemaPage : public QWidget {
    Q_OBJECT

public:
    explicit EsquemaPage(CEsquemaDoc *esquema, QWidget *parent = nullptr);
    explicit EsquemaPage(QWidget *parent = nullptr);

    EsquemaPage() = default;
    ~EsquemaPage();
    QStandardItemModel *model_esquema;
    void newFormula();
    void setMockString(QString mockString) { m_mockString = mockString; }
    CEsquemaDoc* getEsquemaDoc() { return m_esquemaDoc; }

protected:
    Ui::esquemaPage *ui;

    QMap<QStandardItem*, CEsquema*>     m_itemEsquemaMap;
    QMap<QStandardItem*, CFormula*>     m_itemFormulaMap;
    QMap<QListWidgetItem*, CFunction*>  m_itemFunctionMap;
    QMap<QStandardItem*, CData*>        m_itemDataMap;
    newEsquema_dlg* newEsquemadlg = nullptr;

    CFormula    *m_loadedFormula    = nullptr;
    CData       *m_loadedStaticData = nullptr;
    CFunction   *m_activeFunction   = nullptr;
    QString      m_mockString;
    CEsquemaDoc *m_esquemaDoc;

    void newStaticData();
    void loadEsquema();

public:
    void loadFunction();
    inline void updateFunctionProcess(); // Launches apply formula, updates function editor general fields and paints displayed text to keep track of your results.

public slots:
    void handleRemoveSecondLevel(const int index, const QModelIndex &parentIndex);

private slots:
    // VIEWS
    void on_treeView_Esquema_clicked(const QModelIndex &index);
    void on_listWidget_formula_itemSelectionChanged();
    void on_pushButton_addFormula_clicked();
    void on_pushButton_addStaticData_clicked();
    // MANAGE FUNCTIONS
    void on_btn_newFunction_clicked();
    void on_lineEdit_functionName_textChanged(const QString &arg1);
    void on_pushButton_deleteFunctin_clicked();

    // STACKED BOX UI
    // FIND FUNCTION UI
    void on_lineEdit_textToFind_textChanged(const QString &arg1);
    void on_comboBox_setIndexAt_currentIndexChanged(int index);
    void on_comboBox_startFrom_currentIndexChanged(int index);
    // EXTRACTING FUNCTION UI
    void on_lineEdit_endingString_textChanged(const QString &arg1);
    void on_comboBox_readDirection_currentIndexChanged(int index);
    void on_lineEdit_charsToAllow_textEdited(const QString &arg1);
    void on_lineEdit_charsToAvoid_textEdited(const QString &arg1);
    // MOVE LINES UI
    void on_spinBox_moveLinesNum_valueChanged(int arg1);
    void on_comboBox_placeInLine_activated(int index);
    //APPEND STRING UI
    void on_lineEdit_stringToAppend_textChanged(const QString &arg1);
    void on_radioButton_preppend_clicked();
    void on_radioButton_append_clicked();
    //MOVE INDEX UI
    void on_spinBox_moveIndex_valueChanged(int arg1);
    void on_comboBox_typeOfData_currentIndexChanged(int index);

    void handle_newFunActions(CFunction::FunctionType functionType);
    void on_plainTextEdit_staticDataString_textChanged();

    void handleFunctionItemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void handleItemEditFinish(const QModelIndex &index, const QString &text);

signals:
    void functionUpdated(CFormula::IndexPosition index, QString result);
};

#endif // PESQUEMAPAGE_H
