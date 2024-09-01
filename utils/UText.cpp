/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "UText.h"

#include <QRegularExpression>
#include <QString>

namespace UText {
    // Allows for scape characters on the text edit widgets (right now only /n is supported since there shouldn't be a need for any other)
    QString parseFromText(const QString& text) {
        QString parsedText;
        for(int i{ 0 }; i < text.length(); i++) {
            if(text.at(i) == '\\' && i < text.length() - 1) {
                if(text.at(i+1) == 'n') {
                    parsedText.append('\n');
                    i++;
                }
                else parsedText.append(text.at(i));
            }
            else parsedText.append(text.at(i));
        }

        return parsedText;
    }

    QString parseToText(const QString& text) {
        QString parsedText;
        for(int i{ 0 }; i < text.length(); i++) {
            if(text == '\n') {
                parsedText.append("\\n");
            }
            else parsedText.append(text.at(i));
        }
        return parsedText;
    }

    //Replaces text for a placeholder using regex
    void replacePlaceholders(QString& targetString, const QString& regexStr, std::function<std::optional<QString>(const QString)> replacer) {
        QRegularExpression regex(regexStr);
        QRegularExpressionMatchIterator matches = regex.globalMatch(targetString);

        QVector<QPair<int, int>> positions; // Store positions of matches
        QVector<QString> replacements;      // Store corresponding replacements

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString capturedString = match.captured(1);

            // Get the replacement value
            std::optional<QString> replacementOpt = replacer(capturedString);

            if (replacementOpt.has_value()) {
                // Store the start position and length of the match
                positions.push_back(qMakePair(match.capturedStart(0), match.capturedLength(0)));
                replacements.push_back(*replacementOpt); // Store the replacement string
            }
        }

        // Perform the replacements in reverse order to avoid affecting subsequent matches
        for (int i = positions.size() - 1; i >= 0; --i) {
            const QPair<int, int>& pos = positions[i];
            targetString.replace(pos.first, pos.second, replacements[i]);
        }
    }

}
