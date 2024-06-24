#ifndef DDISPLAYPARSEDINFO_H
#define DDISPLAYPARSEDINFO_H

#include "qstandarditemmodel.h"
#include <QDialog>

namespace Ui { class DDisplayParsedInfo; }

class DDisplayParsedInfo : public QDialog {
    Q_OBJECT

public:
    explicit DDisplayParsedInfo(QStandardItemModel *combinedModel, QWidget *parent = nullptr);
    ~DDisplayParsedInfo();
    // Exports the struct to a .csv
    bool exportToCSV();

private slots:
    void on_pushButton_save_clicked() { if(exportToCSV()) { accept(); } }

private:
    Ui::DDisplayParsedInfo *ui;
    QStandardItemModel *m_combinedModel;
};

#endif // DDISPLAYPARSEDINFO_H
