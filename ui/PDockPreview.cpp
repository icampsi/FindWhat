#include "PDockPreview.h"
#include "ui_PDockPreview.h"

#include "PEsquemaPage.h"
#include "document/CMDoc.h"

PDockPreview::PDockPreview(QWidget *parent)
    : QDockWidget(parent), ui(new Ui::PDockPreview)
{
    ui->setupUi(this);
    connect(ui->WToolBar, &WToolBarPreview::previewOptionChanged, this, &PDockPreview::previewOptionChanged);
    ui->stackedWidget_toolBoxOption->setCurrentIndex(0);
}

void PDockPreview::previewOptionChanged(WToolBarPreview::PreviewOption option) {
    ui->stackedWidget_toolBoxOption->setCurrentIndex(static_cast<int>(option));
}

PDockPreview::~PDockPreview() { delete ui; }

void PDockPreview::handleFilePathChanged(const QString &filePath) {
    CPdfDoc *pPdfDoc = CMDoc::getMDoc().newDoc(filePath);
    CMDoc::getMDoc().setActivePdfDoc(pPdfDoc);
    QString docText = pPdfDoc->getText();
    ui->TextDisplayer->setPlainText(docText);

    if(ui->tabWidget_preview->count() > 0) {
        PEsquemaPage *currentPage = dynamic_cast<PEsquemaPage*>(ui->tabWidget_preview->currentWidget());
        if(!currentPage) return;
        currentPage->loadFunction();
        CEsquema* currentEsquema = currentPage->getEsquemaDoc()->getEsquema();
        currentEsquema->generateXSVStructure(docText);
        QString text;
        currentEsquema->xsvm_structureToString(&text, '\"', ',');
        ui->plainTextEdit_resultPreview->setPlainText(text);
    }
}

void PDockPreview::handleFunctionUpdated(CFormula::IndexPosition index, QString result) {
    QBrush brush;
    if(result.isEmpty()) brush = Qt::green; // Green is indexing
    else                 brush = Qt::red;   // Red if exracting

    // Create a QTextCharFormat for the highlighting style
    QTextCharFormat format;
    format.setBackground(brush);
    QTextCursor cursor(ui->TextDisplayer->document());
    // Clear existing highlighting
    QTextCharFormat defaultFormat;
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(defaultFormat);
    // highlight new area
    if(index.initial == index.final) index.final++;
    cursor.setPosition(index.initial);
    cursor.setPosition(index.final, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
}
