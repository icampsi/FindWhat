#include "PEndingStringBlock.h"

#include <QLabel>

#include "utils/generalfunctions.h"

PEndingStringBlock::PEndingStringBlock(QWidget *parent) : QWidget(parent), m_Btn_addEndingStr("+", this), m_lbl_addEndingStr("Ending Strings:", this) {
    setupUi();
}

PEndingStringBlock::~PEndingStringBlock() {
    delete mainLayout;
}

void PEndingStringBlock::setupUi() {

    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Add a button to dynamically add more labels
    QFontMetrics addButtonFontM(m_Btn_addEndingStr.font());
    int addButtonWidth = addButtonFontM.horizontalAdvance(m_Btn_addEndingStr.text()) + 15;
    m_Btn_addEndingStr.setFixedWidth(addButtonWidth);

    mainLayout->addWidget(&m_lbl_addEndingStr);
    mainLayout->addWidget(&m_Btn_addEndingStr);
    connect(&m_Btn_addEndingStr, &QPushButton::clicked, this, [=](){ addNewLabel(true); });

    // Add the initial label, text edit, and button
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    addNewLabel(false);
}

void PEndingStringBlock::addNewLabel(bool attachRemoveBtn) {
    QHBoxLayout *labelLayout = new QHBoxLayout;
    QTextEdit *textEdit = new QTextEdit(this);

    // Display whitespace characters and newlines in QTextEdit
    QTextOption option;
    option.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);
    textEdit->document()->setDefaultTextOption(option);

    textEdit->setFixedHeight(28);
    labelLayout->addWidget(textEdit);
    m_endingStrTxtBlock.push_back(textEdit);

    if(attachRemoveBtn) m_addedLabelLayouts.push_back(labelLayout);

    if(m_function) {
        for(size_t i = m_function->getEndingStringBlock().size(); i <= m_addedLabelLayouts.size(); i++) {
            m_function->addEndingStringBlock("");
        }
    }

    connect(textEdit, &QTextEdit::textChanged, this, [=]() {
        auto it = std::find(m_endingStrTxtBlock.begin(), m_endingStrTxtBlock.end(), textEdit);
        if (it != m_endingStrTxtBlock.end()) {
            size_t i = std::distance(m_endingStrTxtBlock.begin(), it);

            QString parsedText = parseFromText(textEdit->toPlainText());
            m_function->modifyEndingStringBlock(i, parsedText);
            if (!m_blockUpdate) emit functionUpdated();
        }
    });

    if (attachRemoveBtn) {
        QPushButton *removeButton = new QPushButton("-", this);

        // Calculate the width based on the button's text
        QFontMetrics removeBtnFontM(removeButton->font());
        int buttonWidth = removeBtnFontM.horizontalAdvance(removeButton->text()) + 15; // Add padding
        removeButton->setFixedWidth(buttonWidth);
        removeButton->setFixedHeight(28);

        connect(removeButton, &QPushButton::clicked, this, [=](){
            size_t index = removeLabel(labelLayout);
            m_function->deleteEndingStringBlockMember(index);
            if (!m_blockUpdate) emit functionUpdated();
        });

        labelLayout->addWidget(removeButton);
    }

    // Insert the "Add Label" button before the last widget in the main layout
    int lastWidgetIndex = mainLayout->count() - 2; // Index of the before-last widget
    mainLayout->insertLayout(lastWidgetIndex, labelLayout);
}

size_t PEndingStringBlock::removeLabel(QHBoxLayout *labelLayout) {
    QLayoutItem *item;
    int index = -1; // If still negative when returned something went wrong
    while ((item = labelLayout->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget();

        // Remove widgets. If it is the text edit, also remove it from m_endingStrTxtBlock;
        QTextEdit* textEditW = qobject_cast<QTextEdit*>(widget);
        if(textEditW) {
            auto it = std::find(m_endingStrTxtBlock.begin(), m_endingStrTxtBlock.end(), textEditW);
            if (it != m_endingStrTxtBlock.end()) {
                index = std::distance(m_endingStrTxtBlock.begin(), it); // Get the index of "it"
                m_endingStrTxtBlock.erase(it);
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

void PEndingStringBlock::clearBlock() {
    for (auto it = m_addedLabelLayouts.rbegin(); it != m_addedLabelLayouts.rend(); ++it) {
        removeLabel(*it);
    }
    m_addedLabelLayouts.clear();
}

void PEndingStringBlock::updateBlock(CExtractingFunction *function) {
    m_blockUpdate = true; // Blocks unnecessary function updates until every block is clear
    clearBlock();
    m_function = function;
    for(size_t i{ 0 }; i < m_function->getEndingStringBlock().size(); i++) {
        const QString& text = m_function->getEndingStringBlock().at(i);
        if (i == 0) m_endingStrTxtBlock.at(0)->setText(text);
        else {
            addNewLabel(true);
            m_endingStrTxtBlock.at(i)->setText(text);
        }
    }
    m_blockUpdate = false;
    emit functionUpdated();
}
