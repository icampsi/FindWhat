#include "CDocumentSubclasses.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"

void CPdfDoc::PDFToTextPoppler(const QString& inName) {
    // Check if the extension is .pdf. Else, return
    if (!m_filePath.endsWith(".pdf")) {
        return;
    }

    poppler::document* doc = poppler::document::load_from_file(m_filePath.toStdString());
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

    bool previousSpace = false; // Flag to track if the current character is the first in the line
    bool firstChar = true; // Flag to track if the previous character was a space

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line.at(i);
        // Check for newline characters (CR or LF)
        if (c == '\n' || c == '\r') {
            if (!m_text.isEmpty() && m_text.back() == '\n') continue; // Skip consecutive newline characters
            m_text += "\n";
            previousSpace = false;
            firstChar = true;
        }
        // Check for whitespace characters (space, tab, etc.)
        else if (c.isSpace()) {
            if (firstChar) continue; // Skip consecutive spaces
            if (!previousSpace) {
                m_text += ' ';
                previousSpace = true; // Set the previous space flag
            }
        }
        else { // For non-whitespace characters
            m_text += c;
            previousSpace = false;
            firstChar = false;
        }
    }

    outputFile.close(); // Close the output file
    delete doc; // Release pdf file
}

// EXPORTPATHDOC
CExportPathDoc::~CExportPathDoc() {
    for (auto* exportCSV : m_exportCSVs) {
        delete exportCSV;
    }
}

CExportCSV* CExportPathDoc::createExportCSV() {
    m_exportCSVs.emplace_back(new CExportCSV());
    return m_exportCSVs.back();
}

size_t CExportPathDoc::getExportCSVCount() const {
    return m_exportCSVs.size();
}

CExportCSV* CExportPathDoc::getExportCSVByIndex(size_t index) {
    if (index >= m_exportCSVs.size()) {
        return nullptr; // Index out of range
    }
    return m_exportCSVs[index];
}
