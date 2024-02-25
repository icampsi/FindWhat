#ifndef CTEXTEXTRACTOR_H
#define CTEXTEXTRACTOR_H

#include <QString>

class CTextExtractor {
public:
    CTextExtractor();

    // Static function to extract text from pdf's using poppler library
    static void PDFToTextPoppler(const QString& path, QString& text);
};

#endif // CTEXTEXTRACTOR_H
