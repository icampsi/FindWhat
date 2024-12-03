/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTTREEWIDGET_H
#define WEXTTREEWIDGET_H

#include <QTreeWidget>

/*
 * WTreeWidget
 * -----------
 * This class extends QTreeWidget to emit a signal when any key is pressed.
 */
class WTreeWidget : public QTreeWidget {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTTREEWIDGET_H
