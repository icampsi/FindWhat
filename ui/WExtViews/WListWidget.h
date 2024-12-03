/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTLISTWIDGET_H
#define WEXTLISTWIDGET_H

#include <QListWidget>

/*
 * WListWidget
 * -----------
 * This class extends QListWidget to emit a signal when any key is pressed.
 */
class WListWidget : public QListWidget {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTLISTWIDGET_H
