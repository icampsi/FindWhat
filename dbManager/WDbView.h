#ifndef WDBVIEW_H
#define WDBVIEW_H

#include "WTVH.h"
#include "qsortfilterproxymodel.h"
#include "CDbConnection.h"

#include <QSqlTableModel>
#include <QWidget>

namespace Ui { class WDbView; }

class WDbView : public QWidget {
    Q_OBJECT

public:
    explicit WDbView(QWidget *parent = nullptr);
    ~WDbView();

    void changeTable(const QString& tableName);

private slots:
    void on_comboBox_tables_currentIndexChanged(int index);
    void on_treeView_doubleClicked(const QModelIndex &index);
    bool openEditDlg(const QModelIndex &index);
    void filterChanged(int column, const QString& text);

signals:
    void queryChanged();

private:
    Ui::WDbView *ui;
    QSortFilterProxyModel *m_proxyModel;
    WTVH *m_head;
    QVector<QString> insertionOrder;

    CDbConnection m_dbConn;
    QSqlTableModel *m_activeModel;
};

#endif // WDBVIEW_H
