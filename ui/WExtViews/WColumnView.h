/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTCOLUMNVIEW_H
#define WEXTCOLUMNVIEW_H

#include <QColumnView>

/*
 * WColumnView
 * -----------
 * This class extends QColumnView to emit a signal when any key is pressed.
 */
class WColumnView : public QColumnView {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTCOLUMNVIEW_H
