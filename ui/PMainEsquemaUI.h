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
    // CONSTRUCTORS & DESTRUCTORS
    explicit PMainEsquemaUI(QWidget *parent = nullptr);
    ~PMainEsquemaUI();

    // PUBLIC FUNCTIONS
    void newEsquema(CEsquemaDoc* esquemaDoc);
    void changeCurrentPage(PEsquemaPage* page);

private slots:
    void esquemaOptionChanged(WToolBarEsquema::EsquemaOption option);
    void on_pushButton_addEsquema_clicked();
    void on_DeleteEsquema_clicked();
    void on_pushButton_clicked();

public slots:
    void handleDeleteEsquema(const int index);
    void handleFilePathChanged(const QString &filePath);
    void setCurrentPageToEmptyPage(); // Sets the current page to empty page inc ase no esquemas are loaded

private:
    Ui::PMainEsquemaUI *ui;
    PEsquemaPage m_emptyPage; // empty page to load when no esquema is loaded
};
#endif // PMAINESQUEMAUI_H
