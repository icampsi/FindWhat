#ifndef WLOADEDESQUEMES_H
#define WLOADEDESQUEMES_H

#include <QWidget>
#include "PEsquemaPage.h"
#include <QMap>

namespace Ui { class WListWidgetLoadedEsquemes; }

class WLoadedEsquemes : public QWidget {
    Q_OBJECT

public:
    explicit WLoadedEsquemes(QWidget *parent = nullptr);
    ~WLoadedEsquemes();
    void newEsquema(PEsquemaPage *page, CEsquema *esquema);

private slots:
    void on_list_esquemes_itemSelectionChanged();
    void handleDeleteEsquema(const int index);

private:
    Ui::WListWidgetLoadedEsquemes *ui;
    QMap<QListWidgetItem*, PEsquemaPage*> m_itemPageMap;
};

#endif // WLOADEDESQUEMES_H
