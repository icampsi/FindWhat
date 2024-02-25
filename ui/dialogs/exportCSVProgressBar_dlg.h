#ifndef EXPORTCSVPROGRESSBAR_DLG_H
#define EXPORTCSVPROGRESSBAR_DLG_H

#include <QDialog>

namespace Ui { class exportCSVProgressBar_dlg; }

class exportCSVProgressBar_dlg : public QDialog {
    Q_OBJECT

public:
    explicit exportCSVProgressBar_dlg(int progressSize, QWidget *parent = nullptr);
    ~exportCSVProgressBar_dlg();

    void updateProgress();

private:
    Ui::exportCSVProgressBar_dlg *ui;
};

#endif // EXPORTCSVPROGRESSBAR_DLG_H
