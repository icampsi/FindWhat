/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PROGBAR_DLG_H
#define PROGBAR_DLG_H

#include <QDialog>

namespace Ui { class ProgBar_dlg; }

class ProgBar_dlg : public QDialog {
    Q_OBJECT

public:
    explicit ProgBar_dlg(size_t progressSize, const QString& currentWork, QWidget *parent = nullptr);
    ~ProgBar_dlg();

    void updateProgress();
    // Sets the text for the current work description label
    void changeCurrentWork(const QString& currentWork);

private:
    Ui::ProgBar_dlg *ui;
};

#endif // PROGBAR_DLG_H
