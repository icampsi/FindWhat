#ifndef WDOCKPREVIEW_H
#define WDOCKPREVIEW_H

#include <QDockWidget>
#include "ui_WDockPreview.h"
#include "src/CFormula.h"

namespace Ui { class WDockPreview; }

class WDockPreview : public QDockWidget {
    Q_OBJECT

public:
    explicit WDockPreview(QWidget *parent = nullptr);
    ~WDockPreview();

public slots:
    void handleFilePathChanged(const QString &filePath);
    void handleFunctionUpdated(CFormula::IndexPosition index, QString result);

private slots:
    void previewOptionChanged(WToolBarPreview::PreviewOption option);

private:
    Ui::WDockPreview *ui;
};

#endif // WDOCKPREVIEW_H
