/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef INVALIDFILENAME_DLG_H
#define INVALIDFILENAME_DLG_H

#include <QDialog>

namespace Ui { class InvalidFileName_dlg; }

class InvalidFileName_dlg : public QDialog {
    Q_OBJECT

public:
    explicit InvalidFileName_dlg(QWidget *parent, QString *fileName, const QString &oldFilePath);
    ~InvalidFileName_dlg();

private slots:
    void on_buttonBox_accepted();
    void on_lineEdit_newFileName_textChanged(const QString &arg1);

private:
    Ui::InvalidFileName_dlg *ui;
    QString *m_fileName; // Pointer to the old file name for editing it through the editLine widget
};

#endif // INVALIDFILENAME_DLG_H
