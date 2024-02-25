#ifndef WMAINESQUEMAUI_H
#define WMAINESQUEMAUI_H

#include <QWidget>
#include <QStackedWidget>
#include "document/CDocumentSubclasses.h"
#include "WToolBarEsquema.h"
#include "PEsquemaPage.h"

namespace Ui { class WMainEsquemaUI; }

// Edition interface for esquema. Each esquema would have it's own instance of this widget
class WMainEsquemaUI : public QWidget {
    Q_OBJECT

public:
    explicit WMainEsquemaUI(QWidget *parent = nullptr);
    ~WMainEsquemaUI();
    void newEsquema(CEsquemaDoc* esquemaDoc);
    void changeCurrentPage(EsquemaPage* page);

private slots:
    void esquemaOptionChanged(WToolBarEsquema::EsquemaOption option);

    void on_pushButton_addEsquema_clicked();

    void on_DeleteEsquema_clicked();

    void on_pushButton_clicked();

public slots:
    void handleFilePathChanged(const QString &filePath);
private:
    Ui::WMainEsquemaUI *ui;
};
#endif // WMAINESQUEMAUI_H
