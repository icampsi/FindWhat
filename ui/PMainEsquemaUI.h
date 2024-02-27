#ifndef PMAINESQUEMAUI_H
#define PMAINESQUEMAUI_H

#include <QWidget>
#include <QStackedWidget>
#include "document/CDocumentSubclasses.h"
#include "WToolBarEsquema.h"
#include "PEsquemaPage.h"

namespace Ui { class PMainEsquemaUI; }

// Edition interface for esquema. Each esquema would have it's own instance of this widget
class PMainEsquemaUI : public QWidget {
    Q_OBJECT

public:
    explicit PMainEsquemaUI(QWidget *parent = nullptr);
    ~PMainEsquemaUI();
    void newEsquema(CEsquemaDoc* esquemaDoc);
    void changeCurrentPage(PEsquemaPage* page);

private slots:
    void esquemaOptionChanged(WToolBarEsquema::EsquemaOption option);

    void on_pushButton_addEsquema_clicked();

    void on_DeleteEsquema_clicked();

    void on_pushButton_clicked();

public slots:
    void handleFilePathChanged(const QString &filePath);
private:
    Ui::PMainEsquemaUI *ui;
};
#endif // PMAINESQUEMAUI_H
