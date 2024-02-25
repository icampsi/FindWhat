#ifndef WTOOLBARESQUEMA_H
#define WTOOLBARESQUEMA_H

#include "qactiongroup.h"
#include "qtoolbar.h"
#include <QObject>

class WToolBarEsquema : public QToolBar {
    Q_OBJECT
public:
    WToolBarEsquema(QWidget* parent);
    enum class EsquemaOption { EditEsquema, ParseDocument };

signals:
    void previewOptionChanged(WToolBarEsquema::EsquemaOption option);

private slots:
    void handleActionTriggered(QAction *action);

private:
    QActionGroup m_toggleButtonsGroup;
    QAction m_editEsquema;
    QAction m_parseDoc;
};

#endif // WTOOLBARESQUEMA_H
