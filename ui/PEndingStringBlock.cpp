#include "PEndingStringBlock.h"

#include <QLabel>

#include "utils/generalfunctions.h"

PEndingStringBlock::PEndingStringBlock(QWidget *parent) : QWidget(parent), m_Btn_addEndingString("+", this), m_label_addEndingString("Ending Strings:", this) {
    setupUi();
}

PEndingStringBlock::~PEndingStringBlock() {
    delete mainLayout;
}

void PEndingStringBlock::setupUi() {

    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Add a button to dynamically add more labels
    QFontMetrics addButtonFontM(m_Btn_addEndingString.font());
    int addButtonWidth = addButtonFontM.horizontalAdvance(m_Btn_addEndingString.text()) + 15;
    m_Btn_addEndingString.setFixedWidth(addButtonWidth);

    mainLayout->addWidget(&m_label_addEndingString);
    mainLayout->addWidget(&m_Btn_addEndingString);
    connect(&m_Btn_addEndingString, &QPushButton::clicked, this, [=](){ addNewLabel(true); });

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

    if(m_function) m_function->addEndingStringBlock("");

    connect(textEdit, &QTextEdit::textChanged, this, [=]() {
        auto it = std::find(m_endingStrTxtBlock.begin(), m_endingStrTxtBlock.end(), textEdit);
        if (it != m_endingStrTxtBlock.end()) {
            size_t i = std::distance(m_endingStrTxtBlock.begin(), it);

            QString parsedText = parseFromText(textEdit->toPlainText());
            m_function->modifyEndingStringBlock(i, parsedText);
            emit functionUpdated();
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
            QLayoutItem *item;
            while ((item = labelLayout->takeAt(0)) != nullptr) {
                QWidget *widget = item->widget();

                // Remove widgets. If it is the text edit, also remove it from m_endingStrTxtBlock;
                QTextEdit* textEditW = qobject_cast<QTextEdit*>(widget);
                if(textEditW) {
                    auto it = std::find(m_endingStrTxtBlock.begin(), m_endingStrTxtBlock.end(), textEditW);
                    if (it != m_endingStrTxtBlock.end()) {
                        m_endingStrTxtBlock.erase(it);
                        delete textEditW;
                    }
                }
                else if (widget) {
                    delete widget;
                }
                delete item;
            }
            emit functionUpdated();
        });

        labelLayout->addWidget(removeButton);
    }

    // Insert the "Add Label" button before the last widget in the main layout
    int lastWidgetIndex = mainLayout->count() - 2; // Index of the before-last widget
    mainLayout->insertLayout(lastWidgetIndex, labelLayout);
}
