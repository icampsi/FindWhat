/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PDOCKPREVIEW_H
#define PDOCKPREVIEW_H

#include <QDockWidget>
#include "ui_PDockPreview.h"
#include "src/CFormula.h"

namespace Ui { class PDockPreview; }

class PDockPreview : public QDockWidget {
    Q_OBJECT

public:
    explicit PDockPreview(QWidget *parent = nullptr);
    ~PDockPreview();

public slots:
    void handleFilePathChanged(const QString &filePath);
    void handleFunctionUpdated(CFormula::IndexPosition index, QString result);

private slots:
    void previewOptionChanged(WToolBarPreview::PreviewOption option);

    void on_checkBox_showIndividualPages_stateChanged(int arg1);

private:
    Ui::PDockPreview *ui;
};

#endif // PDOCKPREVIEW_H
