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
    void updateBlock(CExtractingFunction *function);
    size_t removeLabel(QHBoxLayout *labelLayout);
    void clearBlock();

signals:
    void functionUpdated();

private:
    CExtractingFunction *m_function = nullptr;

    QVBoxLayout *mainLayout;
    QPushButton  m_Btn_addEndingStr;
    QLabel       m_lbl_addEndingStr;
    bool m_blockUpdate = false; // Flag to avoid unnecessary updates

    std::vector<QTextEdit*>   m_endingStrTxtBlock{};
    std::vector<QHBoxLayout*> m_addedLabelLayouts;
};

#endif // PENDINGSTRINGBLOCK_H
