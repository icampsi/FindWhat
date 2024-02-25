#ifndef WSPREADSHEETTABLE_H
#define WSPREADSHEETTABLE_H

#include <QObject>
#include <qtableview.h>

class WSpreadSheetTable : public QTableView {
    Q_OBJECT
public:
    WSpreadSheetTable(QWidget* parent);
};

#endif // WSPREADSHEETTABLE_H
