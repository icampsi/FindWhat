#ifndef WESQUEMALISTVIEW_H
#define WESQUEMALISTVIEW_H

#include <QListWidget>
#include <QObject>
#include <QKeyEvent>

class WEsquemaListView : public QListWidget {
    Q_OBJECT
public:
    WEsquemaListView(QWidget *parent);
    void keyPressEvent(QKeyEvent *event) override; // Support "del" key for deleting elements

signals:
    void deleteEsquema(const int index);

};

#endif // WESQUEMALISTVIEW_H
