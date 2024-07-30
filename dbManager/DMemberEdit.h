#ifndef DMEMBEREDIT_H
#define DMEMBEREDIT_H

#include <QSqlTableModel>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QDialog>

namespace Ui { class DMemberEdit; }

class DMemberEdit : public QDialog {
    Q_OBJECT

public:
    explicit DMemberEdit(QSqlTableModel  *model, const QModelIndex &modelIndex, QWidget *parent = nullptr);
    ~DMemberEdit();

private:
    bool updateRecord();
    void updateModel(int row, int column);

private slots:
    void submit();

private:
    Ui::DMemberEdit *ui;
    QTableWidget    *m_table;
    QSqlTableModel  *m_model;
    const QModelIndex     &m_index;
};

#endif // DMEMBEREDIT_H
