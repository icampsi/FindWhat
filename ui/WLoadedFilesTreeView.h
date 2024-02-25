#ifndef WLOADEDFILESTREEVIEW_H
#define WLOADEDFILESTREEVIEW_H

#include "qevent.h"
#include "qstandarditemmodel.h"
#include "qtreeview.h"
#include <QWidget>
#include <QMessageBox>

class WLoadedFilesTreeView : public QTreeView {
    Q_OBJECT
public:
    WLoadedFilesTreeView(QWidget* parent);

signals:
    void contentChanged(const std::vector<QString>& paths);

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void emitContentOfTreeView();


protected:
    QStandardItemModel m_model;
};
#endif // WLOADEDFILESTREEVIEW_H
