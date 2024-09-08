/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "ProgBar_dlg.h"
#include "ui_ProgBar_dlg.h"

ProgBar_dlg::ProgBar_dlg(size_t progressSize, const QString& currentWork, QWidget *parent)
    : QDialog(parent), ui(new Ui::ProgBar_dlg)
{
    ui->setupUi(this);
    setWindowTitle("Working...");

    changeCurrentWork(currentWork);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->progressBar->setRange(0, static_cast<int>(progressSize)); // Sets the range of the progress bar to match progressSize variable
    ui->progressBar->setValue(0);
}

ProgBar_dlg::~ProgBar_dlg() {
    delete ui;
}

void ProgBar_dlg::updateProgress() {
    ui->progressBar->setValue(ui->progressBar->value() + 2);
}

void ProgBar_dlg::changeCurrentWork(const QString& currentWork) {
    ui->label_workDescription->setText(currentWork);
}

