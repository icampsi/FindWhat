#ifndef WFORMEXPTOOLBOXPAGE_H
#define WFORMEXPTOOLBOXPAGE_H

#include "document/CExportCSV.h"
#include <QWidget>

namespace Ui { class WExpFormToolBoxPage; }

class WFormExpToolBoxPage : public QWidget { // Pages for Format Export ToolBox
    Q_OBJECT

public:
    explicit WFormExpToolBoxPage(QWidget *parent = nullptr);
    ~WFormExpToolBoxPage();
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

#endif // WFORMEXPTOOLBOXPAGE_H
