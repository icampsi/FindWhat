#include "ExportCSVProgressBar_dlg.h"
#include "ui_ExportCSVProgressBar_dlg.h"

ExportCSVProgressBar_dlg::ExportCSVProgressBar_dlg(int progressSize, QWidget *parent)
    : QDialog(parent), ui(new Ui::exportCSVProgressBar_dlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->progressBar->setRange(0, progressSize); // Sets the range of the progress bar to match progressSize variable
    ui->progressBar->setValue(0);
}

ExportCSVProgressBar_dlg::~ExportCSVProgressBar_dlg() {
    delete ui;
}

void ExportCSVProgressBar_dlg::updateProgress() {
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

