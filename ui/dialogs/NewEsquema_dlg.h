#ifndef NEWESQUEMA_DLG_H
#define NEWESQUEMA_DLG_H

#include <QDialog>

namespace Ui { class newEsquema_dlg; }

// Dialog for creating new esquemes and load them
class newEsquema_dlg : public QDialog {
    Q_OBJECT

public:
    explicit newEsquema_dlg(QWidget *parent, const std::vector<QString>& loadedEsquemaNames);
    ~newEsquema_dlg();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::newEsquema_dlg *ui;
    std::vector<QString> m_loadedEsquemaNames;
};

#endif // NEWESQUEMA_DLG_H
