#ifndef PFORMEXPTOOLBOXPAGE_H
#define PFORMEXPTOOLBOXPAGE_H

#include "document/CExportCSV.h"
#include <QWidget>

namespace Ui { class WExpFormToolBoxPage; }

class PFormExpToolBoxPage : public QWidget { // Pages for Format Export ToolBox
    Q_OBJECT

public:
    explicit PFormExpToolBoxPage(QWidget *parent = nullptr);
    ~PFormExpToolBoxPage();
    void onEsquemaListChanged(const std::vector<QString> &updatedEsquemaDocList);

public slots:
    void handlePathContentChanged(const std::vector<QString>& paths);

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_comboBox_esquemaName_currentIndexChanged(int index);

    void on_checkBox_renameDocs_stateChanged(int arg1);

private:
    Ui::WExpFormToolBoxPage *ui;
    CExportCSV *m_exportCSV;
};

#endif // PFORMEXPTOOLBOXPAGE_H
