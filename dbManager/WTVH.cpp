#include "WTVH.h"

#include <QPainter>
#include <QTreeView>

WTVH::WTVH(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent) {
    setSectionsClickable(true);

    this->setMinimumHeight(50);
    this->setDefaultAlignment(Qt::AlignTop);
}

void WTVH::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {

    if(logicalIndex < searchBoxes.size()){
        calculSearchBoxGeometry(logicalIndex);
    }
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

void WTVH::calculSearchBoxGeometry(int logicalIndex) const {
    int pad   = 4;
    int w     = this->sectionSize(logicalIndex) - pad * 2;
    int h     = 20;
    int aleft = sectionViewportPosition(logicalIndex) + pad;
    int atop  = this->height() / 2;
    QRect rect1(aleft, atop, w, h);
    searchBoxes[logicalIndex]->setGeometry(rect1);
}

void WTVH::handleHeaderDataChanged() {
    for(QLineEdit *box : searchBoxes) {
        delete box;
    }
    searchBoxes.clear();

    for (int i = 0; i < count(); ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setPlaceholderText("Search...");
        connect(lineEdit, &QLineEdit::textChanged, [this, i](const QString &text) {
            emit filterChanged(i, text);
        });
        searchBoxes.append(lineEdit);

        calculSearchBoxGeometry(i);
        searchBoxes[i]->activateWindow();
        searchBoxes[i]->show();
    }
}
