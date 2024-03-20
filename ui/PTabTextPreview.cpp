/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PTabTextPreview.h"

PTabTextPreview::PTabTextPreview(QWidget *parent) : QTextEdit(parent) {
    setReadOnly(true);


    QTextOption option;
    option.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);
    document()->setDefaultTextOption(option);
}

PTabTextPreview::PTabTextPreview(const QString &text, QWidget *parent) : QTextEdit(parent) {
    setReadOnly(true);

    QTextOption option;
    option.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);
    document()->setDefaultTextOption(option);

    setPlainText(text);
}

PTabTextPreview::~PTabTextPreview() {}

void PTabTextPreview::setPreviewText(const QString& text) {
    setPlainText(text);
}
