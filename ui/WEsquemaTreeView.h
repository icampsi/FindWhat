#ifndef WESQUEMATREEVIEW_H
#define WESQUEMATREEVIEW_H
#include <QApplication>
#include <QTreeView>
#include <QStandardItem>
#include <QKeyEvent>
#include <QDebug>

#include "QStandardItemModel"

class WEsquemaTreeView : public QTreeView {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override; // Support "del" key for deleting child elements

public:
    WEsquemaTreeView(QWidget *parent = nullptr);

signals:
    void itemChanged(const QModelIndex &index, const QString &newText);
    void itemEditingFinished(const QModelIndex &index, const QString &text);
    void removeSecondLevel(const int index, const QModelIndex &parentIndex);

private slots:
    void handleDoubleClick(const QModelIndex &index);
    void handleEditingFinished(const QModelIndex &index, const QString &text);
};

#endif // WESQUEMATREEVIEW_H

