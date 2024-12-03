/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WEXTLISTVIEW_H
#define WEXTLISTVIEW_H

#include <QListView>

/*
 * WListView
 * ---------
 * This class extends QListView to emit a signal when any key is pressed.
 */
class WListView : public QListView {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override { emit keyPressed(event); }

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // WEXTLISTVIEW_H
