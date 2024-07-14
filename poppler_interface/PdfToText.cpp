/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "PdfToText.h"
#include "../utils/CPagedText.h"

#include <QString>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"

namespace Poppler_interface {
    void loadPdfDocument(const QString& filePath, CPagedText *pgDoc) {
        poppler::document* doc = poppler::document::load_from_file(filePath.toStdString());
        if (!doc) {
            // Handle error: Unable to load PDF
            qDebug() << "Couldn't open pdf doc. Returned empty text";
            return;
        }
        const int pagesNbr = doc->pages(); // Number of pages

        // Extract text page by page and store it inside m_pages
        std::vector<QString> pages;
        for (int i = 0; i < pagesNbr; ++i) {
            QString pageText = doc->create_page(i)->text().to_utf8().data();
            pgDoc->push_back(std::move(pageText), true);
        }
        delete doc;
        qDebug() << "File Created: " << filePath;
    }

    void saveAsText(CPagedText& pgDoc, QString filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Failed to open file" << filePath;
            return;
        }

        QTextStream out(&file);
        out << pgDoc.getFullText();
        file.close();
    }
}
