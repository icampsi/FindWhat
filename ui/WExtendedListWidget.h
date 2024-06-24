#ifndef WEXTENDEDLISTWIDGET_H
#define WEXTENDEDLISTWIDGET_H

#include <QObject>
#include <QListWidget>

class WExtendedListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit WExtendedListWidget(QWidget *parent = nullptr);
    ~WExtendedListWidget() {}

    void keyPressEvent(QKeyEvent *event) override;

signals:
    void itemDeletitionRequested(const int index);
    void addNewItemRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void renameItem(const QModelIndex &index) { edit(index); }
};

#endif // WEXTENDEDLISTWIDGET_H
