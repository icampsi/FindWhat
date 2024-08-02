#include "WSearcherHeader.h"

#include <QPainter>
#include <QLineEdit>
WSearcherHeader::WSearcherHeader(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent), m_searchBoxEnabled(true) {
    setSectionsClickable(true);
    this->setMinimumHeight(50);
    this->setDefaultAlignment(Qt::AlignTop);
}

void WSearcherHeader::searchBoxEnabled(bool enabled) {
    if(enabled) enableSB();
    else        disableSB();
}

void WSearcherHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {
    if(logicalIndex < m_searchBoxes.size()){
        calculSearchBoxGeometry(logicalIndex);
    }

    QHeaderView::paintSection(painter, rect, logicalIndex);
}

void WSearcherHeader::calculSearchBoxGeometry(int logicalIndex) const {
    if(!m_searchBoxEnabled) return;

    int pad   = 4;
    int w     = this->sectionSize(logicalIndex) - pad * 2;
    int h     = 20;
    int aleft = sectionViewportPosition(logicalIndex) + pad;
    int atop  = this->height() / 2;
    QRect rect1(aleft, atop, w, h);
    m_searchBoxes[logicalIndex]->setGeometry(rect1);
}

void WSearcherHeader::createSB() {
    if(!m_searchBoxEnabled) return;

    clearSB();

    for (int i = 0; i < count(); ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setPlaceholderText("Search...");
        connect(lineEdit, &QLineEdit::textChanged, [this, i](const QString &text) {
            emit filterChanged(i, text);
        });
        m_searchBoxes.append(lineEdit);

        calculSearchBoxGeometry(i);
        m_searchBoxes[i]->activateWindow();
        m_searchBoxes[i]->show();
    }
}

void WSearcherHeader::clearSB() {
    for(QLineEdit *box : m_searchBoxes) {
        delete box;
    }
    m_searchBoxes.clear();
}

void WSearcherHeader::enableSB() {
    m_searchBoxEnabled = true;
    createSB();
    this->setMinimumHeight(50);
    this->update();
}

void WSearcherHeader::disableSB() {
    m_searchBoxEnabled = false;
    clearSB();
    this->setMinimumHeight(0);
    this->update();
}
