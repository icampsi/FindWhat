/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef WTEXTEDDYNAMICBLK_H
#define WTEXTEDDYNAMICBLK_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextOption>

#include "finder/CFunctionClasses.h"

class WTextEdDynamicBlk : public QWidget {
    Q_OBJECT

public:
    explicit WTextEdDynamicBlk(QWidget *parent = nullptr);
    ~WTextEdDynamicBlk();

    void setupUi();

    void addNewLabel(bool attachRemoveBtn = true);
    int removeLabel(QHBoxLayout *labelLayout);

    // Getters&Setters
    void setParentFunction(CExtractingFunction *pFunction) { m_function = pFunction; }
    void updateBlock(const std::vector<QString>& content);
    void clearBlock();
    void setLableText(const QString& text) { m_mainLbl.setText(text); }

    void setSecondaryLabelText(const QString& text) { m_secondaryLblTxt = text; }

signals:
    void blockUpdated();
    void labelAdded(size_t newLabelCount);
    void labelDeleted(size_t i);
    void labelTextChanged(size_t i, const QString& str);

private:
    CExtractingFunction *m_function = nullptr;

    QVBoxLayout *mainLayout;
    QPushButton  m_Btn_addBlock;
    QLabel       m_mainLbl;
    bool         m_blockUpdate = false; // Flag to avoid unnecessary updates
    QString      m_secondaryLblTxt;

    std::vector<QTextEdit*>   m_textEdtBlock{};
    std::vector<QHBoxLayout*> m_addedLabelLayouts;
};

#endif // WTEXTEDDYNAMICBLK_H
