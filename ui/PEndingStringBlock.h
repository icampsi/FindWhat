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
    // void updateBlock(CExtractingFunction *function);
    void updateBlock(const std::vector<QString>& content);
    size_t removeLabel(QHBoxLayout *labelLayout);
    void clearBlock();

    void setLableText(const QString& text) { m_lbl_addEndingStr.setText(text); }

signals:
    void functionUpdated();
    // NEW BOOKMARK
    void labelAdded(size_t newLabelCount);
    void labelDeleted(size_t i);
    void labelTextChanged(size_t i, const QString& str);

private:
    CExtractingFunction *m_function = nullptr;

    QVBoxLayout *mainLayout;
    QPushButton  m_Btn_addEndingStr;
    QLabel       m_lbl_addEndingStr;
    bool         m_blockUpdate = false; // Flag to avoid unnecessary updates

    std::vector<QTextEdit*>   m_endingStrTxtBlock{};
    std::vector<QHBoxLayout*> m_addedLabelLayouts;
};

#endif // PENDINGSTRINGBLOCK_H
