/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTTREEVIEW_H
#define WEXTTREEVIEW_H

#include <QTreeView>

/*
 * WTreeView
 * ---------
 * This class extends QTreeView to emit a signal when any key is pressed.
 */
class WTreeView : public QTreeView {
    Q_OBJECT
public:
    WTreeView(QWidget* parent) : QTreeView(parent){}

protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTTREEVIEW_H
