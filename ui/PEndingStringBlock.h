#ifndef PENDINGSTRINGBLOCK_H
#define PENDINGSTRINGBLOCK_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextOption>
#include <QFontMetrics>

#include "src/CFunctionClasses.h"

class PEndingStringBlock : public QWidget {
    Q_OBJECT

public:
    explicit PEndingStringBlock(QWidget *parent = nullptr);
    ~PEndingStringBlock();

    void setupUi();

    void addNewLabel(bool attachRemoveBtn = true);

    // Getters&Setters
    void setParentFunction(CExtractingFunction *pFunction) { m_function = pFunction; }

signals:
    void functionUpdated();

private:
    CExtractingFunction *m_function = nullptr;

    QVBoxLayout *mainLayout;
    QPushButton  m_Btn_addEndingString;
    QLabel       m_label_addEndingString;

    std::vector<QTextEdit*> m_endingStrTxtBlock{};
};

#endif // PENDINGSTRINGBLOCK_H
