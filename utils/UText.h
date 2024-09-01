/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef UTEXT_H
#define UTEXT_H
#include <optional>

class QString;

namespace std {
    template <typename Signature>
    class function;
}

namespace UText {
    // Allows for scape characters on the text edit widgets (right now only /n is supported since there shouldn't be a need for any other)
    QString parseFromText(const QString& text);
    QString parseToText(const QString& text);

    //Replaces text for a placeholder using regex
    void replacePlaceholders(QString& targetString, const QString& regexStr, std::function<std::optional<QString>(const QString)> replacer);
}

#endif // UTEXT_H
