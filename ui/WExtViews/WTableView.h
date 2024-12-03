/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTTABLEVIEW_H
#define WEXTTABLEVIEW_H

#include <QTableView>

/*
 * WTableView
 * ----------
 * This class extends QTableView to emit a signal when any key is pressed.
 */
class WTableView : public QTableView {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTTABLEVIEW_H
