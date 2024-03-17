/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PROGBAREXPORT_DLG_H
#define PROGBAREXPORT_DLG_H

#include <QDialog>

namespace Ui { class ProgBarExport_dlg; }

class ProgBarExport_dlg : public QDialog {
    Q_OBJECT

public:
    explicit ProgBarExport_dlg(size_t progressSize, QWidget *parent = nullptr);
    ~ProgBarExport_dlg();

    void updateProgress();

private:
    Ui::ProgBarExport_dlg *ui;
};

#endif // PROGBAREXPORT_DLG_H
