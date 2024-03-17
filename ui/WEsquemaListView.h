/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WESQUEMALISTVIEW_H
#define WESQUEMALISTVIEW_H

#include <QListWidget>
#include <QObject>
#include <QKeyEvent>

class WEsquemaListView : public QListWidget {
    Q_OBJECT
public:
    WEsquemaListView(QWidget *parent);
    void keyPressEvent(QKeyEvent *event) override; // Support "del" key for deleting elements

signals:
    void deleteEsquema(const int index);

public slots:
    void handleDeleteEsquema(const size_t index) {
        // Remove the item from the list widget
        QListWidgetItem *item = takeItem(static_cast<int>(index));
        delete item;
    }
};

#endif // WESQUEMALISTVIEW_H
