/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "WTextEdDynamicBlk.h"

#include <QLabel>

#include "utils/UText.h"

WTextEdDynamicBlk::WTextEdDynamicBlk(QWidget *parent) : QWidget(parent), m_Btn_addBlock("+", this), m_mainLbl("UNDEFINED TEXT", this), m_secondaryLblTxt("") {
    setupUi();
}

WTextEdDynamicBlk::~WTextEdDynamicBlk() { delete mainLayout; }

void WTextEdDynamicBlk::setupUi() {
    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Button to dynamically add more labels
    QFontMetrics addButtonFontM(m_Btn_addBlock.font());
    int addButtonWidth = addButtonFontM.horizontalAdvance(m_Btn_addBlock.text()) + 15;
    m_Btn_addBlock.setFixedWidth(addButtonWidth);

    // Add lable and button to the layout
    mainLayout->addWidget(&m_mainLbl);
    mainLayout->addWidget(&m_Btn_addBlock);

    // Connect button to addNewLabel function
    connect(&m_Btn_addBlock, &QPushButton::clicked, this, [=](){ addNewLabel(true); });

    // Add the initial label, text edit, and button
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    addNewLabel(false);
}

void WTextEdDynamicBlk::addNewLabel(bool isSecondary/*flag to avoid atttaching remove button to first lable*/) {
    QHBoxLayout *labelLayout = new QHBoxLayout;

    // Create and style the textEdit
    QTextEdit   *textEdit = new QTextEdit(this);
    QTextOption option;
    option.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);
    textEdit->document()->setDefaultTextOption(option);
    textEdit->setMinimumHeight(400);
    textEdit->setFixedHeight(28);

    // Attach remove button and labels if it's not the main label
    if(isSecondary) {
        QLabel      *secondaryLabel = new QLabel(m_secondaryLblTxt, this);
        labelLayout->addWidget(secondaryLabel);
        m_addedLabelLayouts.push_back(labelLayout);
    }

    // Add the textEdit
    labelLayout->addWidget(textEdit);
    m_textEdtBlock.push_back(textEdit);

    emit labelAdded(m_addedLabelLayouts.size());

    connect(textEdit, &QTextEdit::textChanged, this, [=]() {
        auto it = std::find(m_textEdtBlock.begin(), m_textEdtBlock.end(), textEdit);
        if (it != m_textEdtBlock.end()) {
            size_t i = std::distance(m_textEdtBlock.begin(), it);

            QString parsedText = UText::parseFromText(textEdit->toPlainText());
            emit labelTextChanged(i, parsedText);
            if (!m_blockUpdate) emit blockUpdated();
        }
    });

    if (isSecondary) {
        QPushButton *removeButton = new QPushButton("-", this);

        // Calculate the width based on the button's text
        QFontMetrics removeBtnFontM(removeButton->font());
        int buttonWidth = removeBtnFontM.horizontalAdvance(removeButton->text()) + 15; // Add padding
        removeButton->setFixedWidth(buttonWidth);
        removeButton->setFixedHeight(28);

        connect(removeButton, &QPushButton::clicked, this, [=](){
            size_t index = removeLabel(labelLayout);
            emit labelDeleted(index);
            if (!m_blockUpdate) emit blockUpdated();
        });

        labelLayout->addWidget(removeButton);
    }

    // Insert the "Add Label" button before the last widget in the main layout
    int lastWidgetIndex = mainLayout->count() - 2; // Index of the before-last widget
    mainLayout->insertLayout(lastWidgetIndex, labelLayout);
}

int WTextEdDynamicBlk::removeLabel(QHBoxLayout *labelLayout) {
    QLayoutItem *item;
    int index = -1; // If still negative when returned, something went wrong
    while ((item = labelLayout->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget();

        // Remove widgets. If it is the text edit, also remove it from m_endingStrTxtBlock;
        QTextEdit* textEditW = qobject_cast<QTextEdit*>(widget);
        if(textEditW) {
            auto it = std::find(m_textEdtBlock.begin(), m_textEdtBlock.end(), textEditW);
            if (it != m_textEdtBlock.end()) {
                index = std::distance(m_textEdtBlock.begin(), it); // Get the index of "it"
                m_textEdtBlock.erase(it);
                delete textEditW;
            }
        }
        else if (widget) {
            delete widget;
        }
        delete item;
    }
    return index;
}

void WTextEdDynamicBlk::clearBlock() {
    for (auto it = m_addedLabelLayouts.rbegin(); it != m_addedLabelLayouts.rend(); ++it) {
        removeLabel(*it);
    }
    m_addedLabelLayouts.clear();
}

void WTextEdDynamicBlk::updateBlock(const std::vector<QString>& content) {
    m_blockUpdate = true; // Blocks unnecessary function updates until every block is clear
    clearBlock();
    for(size_t i{ 0 }; i < content.size(); i++) {
        const QString& text = content.at(i);
        if (i == 0) m_textEdtBlock.at(0)->setText(text);
        else {
            addNewLabel(true);
            m_textEdtBlock.at(i)->setText(text);
        }
    }
    m_blockUpdate = false;
    emit blockUpdated();
}
