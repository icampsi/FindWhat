#ifndef WSEARCHERHEADER_H
#define WSEARCHERHEADER_H

#include <QHeaderView>

class QLineEdit;
// Header with an added option to add a search box on each column.
class WSearcherHeader : public QHeaderView {
    Q_OBJECT

public:
    explicit WSearcherHeader(Qt::Orientation orientation, QWidget *parent = nullptr);
    void searchBoxEnabled(bool enabled);

private:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void calculSearchBoxGeometry(int logicalIndex) const;
    void createSB(); // Creates searchBoxes and pleaces them on each column
    void clearSB();  // Deletes all searchBoxes
    void enableSB();
    void disableSB();

signals:
    void filterChanged(int column, const QString &text); // Emmited when the content of a searchbox changes

public slots:
    void updateHeader() { createSB(); }

private:
    QVector<QLineEdit*> m_searchBoxes; // SearchBoxes list for each column
    bool m_searchBoxEnabled;
};

#endif // WSEARCHERHEADER_H
