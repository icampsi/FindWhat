#ifndef WTVH_H
#define WTVH_H

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QHeaderView>

class WTVH : public QHeaderView {
    Q_OBJECT

public:
    explicit WTVH(Qt::Orientation orientation, QWidget *parent = nullptr);
    void updateHeader() { handleHeaderDataChanged(); }

private:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void calculSearchBoxGeometry(int logicalIndex) const;

signals:
    void filterChanged(int column, const QString &text);

public slots:
    void handleHeaderDataChanged();

private:
    QVector<QLineEdit*> searchBoxes;
};

#endif // WTVH_H
