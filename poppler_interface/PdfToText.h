/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PDFTOTEXT_H
#define PDFTOTEXT_H

class CPagedText;
class QString;

namespace Poppler_interface {
    void loadPdfDocument(const QString& filePath, CPagedText *pgDoc);
    void saveAsText(CPagedText& pgDoc, QString filePath);
}


#endif // PDFTOTEXT_H
