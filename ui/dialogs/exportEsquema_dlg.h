// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#ifndef EXPORTESQUEMA_DLG_H
#define EXPORTESQUEMA_DLG_H

#include <QDialog>

namespace Ui {
class exportEsquema_dlg;
}

class exportEsquema_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit exportEsquema_dlg(QWidget *parent, size_t *pSelectedIndex);
    ~exportEsquema_dlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::exportEsquema_dlg *ui;
    size_t *m_selectedIndex;
};

#endif // EXPORTESQUEMA_DLG_H
