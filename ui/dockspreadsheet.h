#ifndef DOCKSPREADSHEET_H
#define DOCKSPREADSHEET_H

#include <QDockWidget>

namespace Ui {
class dockSpreadSheet;
}

class dockSpreadSheet : public QDockWidget
{
    Q_OBJECT

public:
    explicit dockSpreadSheet(QWidget *parent = nullptr);
    ~dockSpreadSheet();

private:
    Ui::dockSpreadSheet *ui;
};

#endif // DOCKSPREADSHEET_H
