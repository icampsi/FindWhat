#ifndef EXPORTESQUEMA_DLG_H
#define EXPORTESQUEMA_DLG_H

#include <QDialog>

namespace Ui {
class exportEsquema_dlg;
}

class exportEsquema_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit exportEsquema_dlg(QWidget *parent, int *pSelectedIndex);
    ~exportEsquema_dlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::exportEsquema_dlg *ui;
    int *m_selectedIndex;
};

#endif // EXPORTESQUEMA_DLG_H
