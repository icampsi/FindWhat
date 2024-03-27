/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PFORMEXPTOOLBOXPAGE_H
#define PFORMEXPTOOLBOXPAGE_H

#include "document/CExportCSV.h"
#include "ui/ui_PFormExpToolBoxPage.h"
#include <QWidget>

namespace Ui { class PExpFormToolBoxPage; }

class PFormExpToolBoxPage : public QWidget { // Pages for Format Export ToolBox
    Q_OBJECT

public:
    // CONSTRUCTORS&DESTRUCTORS
    explicit PFormExpToolBoxPage(QWidget *parent = nullptr, CExportCSV *exportCSV = nullptr);
    ~PFormExpToolBoxPage() { delete ui; }

    // PUBLIC FUNCTIONS
    void onEsquemaListChanged(const std::vector<QString> &updatedEsquemaDocList);
    void updateFields();

public slots:
    void handlePathContentChanged(const std::vector<QString>& paths) { m_exportCSV->setPathFiles(paths); }

private slots:
    void on_comboBox_esquemaName_currentIndexChanged(int index);
    void on_checkBox_renameDocs_stateChanged(int arg1);
    void on_lineEdit_formatString_textChanged(const QString &arg1) { m_exportCSV->setCSVFormat(arg1); }
    void on_lineEdit_renameDocs_textChanged(const QString &arg1)   { m_exportCSV->setFileNamePlaceholder(arg1); }
    void on_lineEdit_actOnlyIf_textChanged(const QString &arg1)    { m_exportCSV->setIdText(arg1); }

private:
    // MEMBERS
    Ui::PExpFormToolBoxPage *ui;
    CExportCSV *m_exportCSV;
};

#endif // PFORMEXPTOOLBOXPAGE_H
