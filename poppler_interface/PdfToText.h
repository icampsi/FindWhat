#ifndef PDFTOTEXT_H
#define PDFTOTEXT_H

class CPagedText;
class QString;

namespace Poppler_interface {
    void loadPdfDocument(const QString& filePath, CPagedText *pgDoc);
}


#endif // PDFTOTEXT_H
