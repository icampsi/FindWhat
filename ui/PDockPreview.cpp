/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PDockPreview.h"
#include "ui_PDockPreview.h"

#include "document/CMDoc.h"
#include "PTabTextPreview.h"

PDockPreview::PDockPreview(QWidget *parent, CPdfDoc *previewDoc)
    : QDockWidget(parent), ui(new Ui::PDockPreview), m_previewDoc{previewDoc}
{
    ui->setupUi(this);
    connect(ui->WToolBar, &WToolBarPreview::previewOptionChanged, this, &PDockPreview::previewOptionChanged);
    ui->stackedWidget_toolBoxOption->setCurrentIndex(0);
    PTabTextPreview *tab = new PTabTextPreview();
    ui->tabWidget_preview->addTab(tab, "Preview Text");
}

void PDockPreview::previewOptionChanged(WToolBarPreview::PreviewOption option) {
    ui->stackedWidget_toolBoxOption->setCurrentIndex(static_cast<int>(option));
}

PDockPreview::~PDockPreview() { delete ui; }

void PDockPreview::handleFilePathChanged(const QString &filePath) {
    if(m_previewDoc) {
        delete m_previewDoc;
    }
    m_previewDoc = CMDoc::getMDoc().newDoc(filePath);
    CMDoc::getMDoc().setActivePdfDoc(m_previewDoc);

    setPreviewText();
}

void PDockPreview::handleFunctionUpdated(CFormula::IndexPosition index, const QString& result) {
    // BOOKMARK - Since last change in cformula after extracting data all indexing function stay red. Need to change result for function type
    if(result.isEmpty()) m_highlightBrush = Qt::green; // Green is indexing
    else                 m_highlightBrush = Qt::red;   // Red if exracting
    updateCursor(index);
}

void PDockPreview::updateCursor(CFormula::IndexPosition index) {
    m_indexPosition = index;
    if(index.initial == index.final) index.final++;

    // Create a QTextCharFormat for the highlighting style
    QTextCharFormat format;
    format.setBackground(m_highlightBrush);

    // Clear existing highlighting on every tab
    QTextCharFormat defaultFormat;
    for(int i{0}; i < ui->tabWidget_preview->count(); i++) {
        QTextCursor cursor(static_cast<PTabTextPreview*>(ui->tabWidget_preview->widget(i))->document());
        cursor.select(QTextCursor::Document);
        cursor.setCharFormat(defaultFormat);
    }

    // Sets the cursor to the needed tab depending on index.
    int highlightedTabIndex; // To keep track of the highlighted tab so we can automatically scroll to it later
    QTextCursor cursor;
    CPdfDoc::RelativeIndex from_relativeIndex;
    CPdfDoc::RelativeIndex to_relativeIndex;
    if(ui->tabWidget_preview->count() == 1) { // If there is only one tab
        from_relativeIndex.charIndex = index.initial;
        to_relativeIndex.charIndex = index.final;

        highlightedTabIndex = 0;
        cursor =  QTextCursor(static_cast<PTabTextPreview*>(ui->tabWidget_preview->widget(highlightedTabIndex))->document());
    } else { // If there are multiple pages on different tabs
        from_relativeIndex = m_previewDoc->getPageRelativeIndex(index.initial);
        to_relativeIndex   = m_previewDoc->getPageRelativeIndex(index.final);

        highlightedTabIndex = static_cast<int>(from_relativeIndex.pageIndex);
        cursor = QTextCursor(static_cast<PTabTextPreview*>(ui->tabWidget_preview->widget(highlightedTabIndex))->document());
    }

    // Highlight the new area
    if(ui->tabWidget_preview->count() == 1 || from_relativeIndex.pageIndex == to_relativeIndex.pageIndex) { // if highlight only affects one page
        cursor.setPosition(static_cast<int>(from_relativeIndex.charIndex));
        cursor.setPosition(static_cast<int>(to_relativeIndex.charIndex), QTextCursor::KeepAnchor);
        cursor.setCharFormat(format);
    } else { // Highlight across pages
        QTextCursor endCursor(static_cast<PTabTextPreview*>(ui->tabWidget_preview->widget(static_cast<int>(to_relativeIndex.pageIndex)))->document());

        // Highlight from the initial position to the end of the initial page
        cursor.setPosition(static_cast<int>(from_relativeIndex.charIndex));
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor); // Highlight up to the end of the page
        cursor.setCharFormat(format);

        // Move to the beginning of the final page
        endCursor.movePosition(QTextCursor::Start);

        // Highlight from the beginning of the final page to the final position
        endCursor.setPosition(static_cast<int>(to_relativeIndex.charIndex) + 1, QTextCursor::KeepAnchor);
        endCursor.setCharFormat(format);

        // Fully highlight middle pages if any
        if(from_relativeIndex.pageIndex != to_relativeIndex.pageIndex - 1) {// Meaning they are not consecutive pages
            for(size_t i = from_relativeIndex.pageIndex + 1; i < to_relativeIndex.pageIndex; i++) {
                QTextCursor middleCursor(static_cast<PTabTextPreview*>(ui->tabWidget_preview->widget(static_cast<int>(i)))->document());
                middleCursor.setPosition(QTextCursor::Start);
                middleCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                middleCursor.setCharFormat(format);
            }
        }
    }

    // Ensure that the highlighted area is visible by scrolling to it
    ui->tabWidget_preview->setCurrentIndex(highlightedTabIndex);
    cursor.setPosition(cursor.anchor());
    QTextEdit *textEdit = static_cast<PTabTextPreview*>(ui->tabWidget_preview->currentWidget());
    textEdit->setTextCursor(cursor);
    textEdit->ensureCursorVisible();
    qDebug() << "Cursor updated";
}

void PDockPreview::on_checkBox_showIndividualPages_stateChanged(int arg1) {
    if (m_previewDoc == nullptr) return;
    if (!arg1) {
        clearTabs();
        PTabTextPreview *tab = new PTabTextPreview(ui->tabWidget_preview);
        ui->tabWidget_preview->addTab(tab, "Preview Text");
    }
    setPreviewText();
    updateCursor(m_indexPosition);
}

void PDockPreview::setPreviewText() {
    if (!ui->checkBox_showIndividualPages->isChecked()) { // For full document display
        QString docText = m_previewDoc->getFullText();
        PTabTextPreview *tab = static_cast<PTabTextPreview*>(ui->tabWidget_preview->currentWidget());
        tab->setPreviewText(docText);
    } else { // For breaking the document into pages
        clearTabs();
        for (size_t i{0}; i < m_previewDoc->pageCount(); i++) {
            QString label = "Page " + QString::number(i + 1);
            PTabTextPreview *tab = new PTabTextPreview(m_previewDoc->getPage(i).pageText, ui->tabWidget_preview);
            ui->tabWidget_preview->addTab(tab, label);
        }
    }
}

void PDockPreview::clearTabs() {
    // Iterate through each tab and delete it
    while (ui->tabWidget_preview->count() > 0) {
        QWidget *widget = ui->tabWidget_preview->widget(0);
        ui->tabWidget_preview->removeTab(0);
        delete widget;
    }
}
