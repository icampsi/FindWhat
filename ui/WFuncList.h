#ifndef WFUNCLIST_H
#define WFUNCLIST_H

#include "ui/PEsquemaPage.h"

#include <QObject>
#include <QListWidget>

class WFuncList : public QListWidget {
    Q_OBJECT
public:
    explicit WFuncList(QWidget *parent = nullptr);
    ~WFuncList() {}

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

#endif // WFUNCLIST_H
