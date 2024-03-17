/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "ProgBarExport_dlg.h"
#include "ui_ProgBarExport_dlg.h"

ProgBarExport_dlg::ProgBarExport_dlg(size_t progressSize, QWidget *parent)
    : QDialog(parent), ui(new Ui::ProgBarExport_dlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->progressBar->setRange(0, static_cast<int>(progressSize)); // Sets the range of the progress bar to match progressSize variable
    ui->progressBar->setValue(0);
}

ProgBarExport_dlg::~ProgBarExport_dlg() {
    delete ui;
}

void ProgBarExport_dlg::updateProgress() {
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

