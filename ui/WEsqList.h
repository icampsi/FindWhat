#ifndef WESQLIST_H
#define WESQLIST_H

#include "ui/PEsquemaPage.h"

#include <QObject>
#include <QListWidget>

class WEsqList : public QListWidget {
    Q_OBJECT
public:
    explicit WEsqList(QWidget *parent = nullptr);
    ~WEsqList() {}

    void keyPressEvent(QKeyEvent *event) override; // Support "del" key for deleting elements

signals:
    void deleteEsquema(const int index);

public:
    void newEsquema(PEsquemaPage *page, CEsquema *esquema);
    void delEsq(const size_t index) {
        QListWidgetItem *item = takeItem(static_cast<int>(index));
        delete item;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void renameItem(const QModelIndex &index) { edit(index); }
    void handleItemChanged(QListWidgetItem *item);
    void handleItemSelectionChanged();

private:
    QMap<QListWidgetItem*, PEsquemaPage*> m_itemPageMap;
};

#endif // WESQLIST_H
