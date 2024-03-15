// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "InvalidFileName_dlg.h"
#include "ui_InvalidFileName_dlg.h"

#include "qpushbutton.h"

#include "utils/GeneralFunctions.h"

InvalidFileName_dlg::InvalidFileName_dlg(QWidget *parent, QString *fileName, const QString &oldFilePath)
    : QDialog(parent), ui(new Ui::InvalidFileName_dlg), m_fileName{fileName} {
    ui->setupUi(this);

    ui->label_oldFileName->setText(oldFilePath);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

InvalidFileName_dlg::~InvalidFileName_dlg() { delete ui; }

void InvalidFileName_dlg::on_buttonBox_accepted() {
    *m_fileName = ui->lineEdit_newFileName->text();
}


void InvalidFileName_dlg::on_lineEdit_newFileName_textChanged(const QString &arg1) {
    if (arg1.size() > 0 && !SystemUtils::containsInvalidFileNameChars(arg1)) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

