#ifndef DDISPLAYPARSEDINFO_H
#define DDISPLAYPARSEDINFO_H

#include "ui/dialogs/ui_DDisplayParsedInfo.h"
#include <QDialog>

namespace Ui { class DDisplayParsedInfo; }

class DDisplayParsedInfo : public QDialog {
    Q_OBJECT

public:
    explicit DDisplayParsedInfo(std::vector<std::vector<QString> > &xsvStruct, QWidget *parent = nullptr);
    ~DDisplayParsedInfo();
    // Populates the table with the recived xsvStucture
    void populateTable(std::vector<std::vector<QString>>& xsvStruct);

private slots:
    // Exports the struct to a .csv
    bool exportToCSV();
    void on_pushButton_save_clicked() { if(exportToCSV()) { accept(); } }
    // Updates the xsvStructure on table edition
    void on_tableWidget_cellChanged(int row, int column) { m_xsvStruct[row][column] = ui->tableWidget->item(row, column)->text(); }

private:
    Ui::DDisplayParsedInfo *ui;
    std::vector<std::vector<QString>> &m_xsvStruct;
};

#endif // DDISPLAYPARSEDINFO_H
