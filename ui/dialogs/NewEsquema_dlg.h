/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef NEWESQUEMA_DLG_H
#define NEWESQUEMA_DLG_H

#include <QDialog>

namespace Ui { class NewEsquema_dlg; }

// Dialog for creating new esquemes and load them
class NewEsquema_dlg : public QDialog {
    Q_OBJECT

public:
    explicit NewEsquema_dlg(QWidget *parent, const std::vector<QString>& loadedEsquemaNames);
    ~NewEsquema_dlg();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::NewEsquema_dlg *ui;
    std::vector<QString> m_loadedEsquemaNames;
};

#endif // NEWESQUEMA_DLG_H
