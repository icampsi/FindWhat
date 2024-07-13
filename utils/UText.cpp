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
    void replacePlaceholders(QString& targetString, const QString& regexStr, std::function<QString(const QString&)> replacer) {
        QRegularExpression regex(regexStr);
        QRegularExpressionMatchIterator matches = regex.globalMatch(targetString);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString capturedString = match.captured(1);

            QString replacement = replacer(capturedString);

            targetString.replace(match.capturedStart(0), match.capturedLength(0), replacement);
            matches = regex.globalMatch(targetString); // Update matches
        }
    }
}
