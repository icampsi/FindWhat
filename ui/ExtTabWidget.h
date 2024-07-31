/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef EXTTABWIDGET_H
#define EXTTABWIDGET_H

#include "qlineedit.h"
#include <QTabWidget>

#include <QTabBar>
class QPushButton;

// TAB BAR ------------------------------------------------------------------------------
class ExtTabBar : public QTabBar {
    Q_OBJECT

public:
    explicit ExtTabBar(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void startEditing(int index);    // Starts edition on tab name

private slots:
    void finishEditing(); // Finished tab name edition

signals:
    void addTabClicked();

    // MEMBERS
private:
    QPushButton *addTabButton;
    QLineEdit *editLine;
    int editingIndex;
};

// TAB WIDGET ------------------------------------------------------------------------------
class ExtTabWidget : public QTabWidget {
    Q_OBJECT

public:
    explicit ExtTabWidget(QWidget *parent = nullptr);
};

#endif // EXTTABWIDGET_H
