/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PDOCKPREVIEW_H
#define PDOCKPREVIEW_H

#include <QDockWidget>
#include "ui_PDockPreview.h"
#include "src/CFormula.h"
#include "document/CPdfDoc.h"

namespace Ui { class PDockPreview; }

class PDockPreview : public QDockWidget {
    Q_OBJECT

public:
    explicit PDockPreview(QWidget *parent = nullptr, CPdfDoc *previewDoc = nullptr);
    ~PDockPreview();

public slots:
    void handleFilePathChanged(const QString &filePath);
    void handleFunctionUpdated(CFormula::IndexPosition index, const QString &result);

private slots:
    void previewOptionChanged(WToolBarPreview::PreviewOption option);
    void on_checkBox_showIndividualPages_stateChanged(int arg1);

private:
    Ui::PDockPreview *ui;
    // MEMBERS
    CPdfDoc *m_previewDoc = nullptr;
    int      m_cursorPos  = 0;
    QBrush   m_highlightBrush = Qt::green;
    CFormula::IndexPosition m_indexPosition{0, 0};

    // PRIVATE FUNCITONS
    void setPreviewText();
    void clearTabs();
    void updateCursor(CFormula::IndexPosition index);
};

#endif // PDOCKPREVIEW_H
