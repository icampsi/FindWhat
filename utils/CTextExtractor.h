#ifndef CTEXTEXTRACTOR_H
#define CTEXTEXTRACTOR_H

#include <QString>
#include <qDebug>

#include "poppler/cpp/poppler-document.h"

class CTextExtractor {
public:
    CTextExtractor(const QString& docName);
    ~CTextExtractor();

    // Static function to extract text from pdf's using poppler library
    static void PDFToTextPoppler(const QString& path, QString& text);

    void getFullText(QString& fullText) const;

    const QString& getPage(const int index) {
        if (index < 0 || index >= m_pages.size()) {
            qDebug() << "Index out of bound for page of document";
        }
        return m_pages.at(index);
    }

private:
    poppler::document* m_doc;
    QString m_docName;
    std::vector<QString> m_pages;
};

#endif // CTEXTEXTRACTOR_H
