#ifndef WTOOLBARPREVIEW_H
#define WTOOLBARPREVIEW_H

#include "qactiongroup.h"
#include "qtoolbar.h"
#include <QObject>

class WToolBarPreview : public QToolBar { // ToolBar for the preview docked window
    Q_OBJECT
public:
    WToolBarPreview(QWidget* parent);
    enum class PreviewOption { TextPreview, ResultPreview, CSVPreview };

signals:
    void previewOptionChanged(WToolBarPreview::PreviewOption option);

private slots:
    void handleActionTriggered(QAction *action);

protected:
    QActionGroup m_toggleButtonsGroup;
    QAction m_textPreview;
    QAction m_resultPreview;
    QAction m_spreadSheetPreview;
};

#endif // WTOOLBARPREVIEW_H
