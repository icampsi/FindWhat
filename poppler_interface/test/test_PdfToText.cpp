/* =================================================== *
 * ====        Copyright (c) 2024 icampsi       ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include <catch2/catch_all.hpp>

#include "../PdfToText.h"
#include "utils/CPagedText.h"

#include <QString>
#include <QDebug>

#include <QDir>
#include <QFileInfoList>

TEST_CASE("Save string as a file") {
//BOOKMARK -
    // This is not an automated test yet. The results can be checked by manually looking at the generated files, but mainly I
    // cretaed this not as a test but as a way to get .txt files to use as test material for the finder.

    // Convert every pdf file inside /pdf into a text file
    const QString project_src = PROJECT_SOURCE_DIR; // Defined in CMakeLists
    QDir dir(project_src + "/pdf");
    if (!dir.exists()) {
        qDebug() << "Directory does not exist!";
        return;
    }

    QStringList filters;
    filters << "*.pdf";

    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

    foreach (const QFileInfo &fileInfo, fileList) {
        CPagedText pgDoc;
        Poppler_interface::loadPdfDocument(fileInfo.absoluteFilePath(), &pgDoc);
        QString newFilePath = fileInfo.baseName() + ".txt";
        Poppler_interface::saveAsText(pgDoc, newFilePath);
    }

    QString filePath = project_src + "/poppler_interface/test/test_pdf.pdf";
    CPagedText pgDoc;
    Poppler_interface::loadPdfDocument(filePath, &pgDoc);
    Poppler_interface::saveAsText(pgDoc, "test_pdf.txt");
}
