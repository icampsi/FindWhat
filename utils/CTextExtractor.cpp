#include "CTextExtractor.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"

CTextExtractor::CTextExtractor() {}

void CTextExtractor::PDFToTextPoppler(const QString& inName, QString& text) {
    // Check if the extension is .pdf. Else, return
    if (!inName.endsWith(".pdf")) {
        return;
    }

    poppler::document* doc = poppler::document::load_from_file(inName.toStdString());
    const int pagesNbr = doc->pages();

    QFile outputFile("textOut.txt");

    // Check if the file is open
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Error opening the file.";
        return;
    }

    QTextStream output(&outputFile);
    QString line;
    for (int i = 0; i < pagesNbr; ++i) {
        line += doc->create_page(i)->text().to_utf8().data();
    }

    bool previousSpace  = false; // Flag to track if the current character is the first in the line
    bool firstChar      = true;  // Flag to track if the previous character was a space

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line.at(i);        // Check for newline characters (CR or LF)
        if (c == '\n' || c == '\r') {
            if (!text.isEmpty() && text.back() == '\n') continue; // Skip consecutive newline characters
            text += "\n";
            previousSpace = false;
            firstChar     = true;
        }
        // Check for whitespace characters (space, tab, etc.)
        else if (c.isSpace()) {
            if (firstChar) continue; // Skip consecutive spaces
            if (!previousSpace) {
                text += ' ';
                previousSpace = true; // Set the previous space flag
            }
        }
        else { // For non-whitespace characters
            text += c;
            previousSpace = false;
            firstChar     = false;
        }
    }
    outputFile.close();
    delete doc;
}
