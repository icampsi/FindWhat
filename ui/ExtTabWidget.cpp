/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "ExtTabWidget.h"
#include "qevent.h"

#include <QPushButton>

// TAB BAR ------------------------------------------------------------------------------
ExtTabBar::ExtTabBar(QWidget *parent)
    : QTabBar(parent), addTabButton(new QPushButton("+", this))
{
    addTabButton->setFixedSize(18, 18);
    addTabButton->setStyleSheet(R"(
        QPushButton {
            background-color: green;
            color: white;
            border: 1px solid darkgreen;
            border-radius: 3px;
            font-weight: bold;
            font-size: 16px;
            padding: 0px;
        }

        QPushButton:hover {
            background-color: limegreen;
        }

        QPushButton:pressed {
            background-color: darkgreen;
        }
    )");
    connect(addTabButton, &QPushButton::clicked, this, &ExtTabBar::addTabClicked);
    addTab("");
    setTabEnabled(0, false);
    setTabButton(0, RightSide, addTabButton);

    editLine = new QLineEdit(this);
    editLine->hide();
}

void ExtTabBar::mouseDoubleClickEvent(QMouseEvent *event) {
    int index = tabAt(event->pos());
    if (count() - 1) {
        if(editLine) finishEditing();
        startEditing(index);
    }
    QTabBar::mouseDoubleClickEvent(event);
}

void ExtTabBar::startEditing(int index) {
    editingIndex = index;
    QString currentText = tabText(index);
    editLine->setText(currentText);
    connect(editLine, &QLineEdit::editingFinished, this, &ExtTabBar::finishEditing);

    QRect tabRect = this->tabRect(index);
    editLine->setGeometry(tabRect);
    editLine->show();
    editLine->setFocus();
    editLine->selectAll();
}

void ExtTabBar::finishEditing() {
    if (editingIndex != -1) {
        QString newText = editLine->text();
        if(newText.isEmpty()) newText = "New Page";
        setTabText(editingIndex, newText);
        editLine->hide();
        editingIndex = -1;
    }
}

// TAB WIDGET ------------------------------------------------------------------------------
ExtTabWidget::ExtTabWidget(QWidget *parent) : QTabWidget(parent) {
    ExtTabBar *tabBar = new ExtTabBar(this);
    setTabBar(tabBar);
}
