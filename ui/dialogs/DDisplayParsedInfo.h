#ifndef DDISPLAYPARSEDINFO_H
#define DDISPLAYPARSEDINFO_H

#include "qstandarditemmodel.h"
#include <QDialog>

namespace Ui { class DDisplayParsedInfo; }

class DDisplayParsedInfo : public QDialog {
    Q_OBJECT

public:
    explicit DDisplayParsedInfo(QAbstractItemModel *combinedModel, QWidget *parent = nullptr);
    ~DDisplayParsedInfo();
    // Exports the struct to a .csv
    bool exportToCSV();
#ifdef ENABLE_DBMANAGER
    bool commitTable();
#endif

private slots:
    void on_pushButton_save_clicked();

private:
    Ui::DDisplayParsedInfo *ui;
    QAbstractItemModel *m_combinedModel;
};

#endif // DDISPLAYPARSEDINFO_H
