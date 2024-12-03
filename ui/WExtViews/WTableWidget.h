/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTTABLEWIDGET_H
#define WEXTTABLEWIDGET_H

#include <QTableWidget>

/*
 * WTableWidget
 * ------------
 * This class extends QTableWidget to emit a signal when any key is pressed.
 */
class WTableWidget : public QTableWidget {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTTABLEWIDGET_H
