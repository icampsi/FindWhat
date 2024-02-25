#include "CMDoc.h"
#include "CDocumentSubclasses.h"

CEsquemaDoc *CMDoc::newDoc(CEsquema *esquema) {
    CEsquemaDoc *pDoc = new CEsquemaDoc(esquema);
    m_loadedEsquemaDocs.push_back(pDoc);
    esquemaListUpdated();
    return pDoc;
}

CPdfDoc* CMDoc::newDoc(const QString& filePath) {
    CPdfDoc *pDoc = new CPdfDoc(filePath);
    m_loadedPdfDocs.push_back(pDoc);
    return pDoc;
}

void CMDoc::onDocumentDestroyed(CDocument *pDoc) {
    if (dynamic_cast<CEsquemaDoc*>(pDoc)) {
        esquemaListUpdated();
        auto it = std::remove_if(m_loadedEsquemaDocs.begin(), m_loadedEsquemaDocs.end(),
                                 [pDoc](const CDocument *ptr) { return ptr == pDoc; });
        m_loadedEsquemaDocs.erase(it, m_loadedEsquemaDocs.end());
        delete pDoc; // Delete the destroyed document
    } else if (dynamic_cast<CPdfDoc*>(pDoc)) {
        auto it = std::remove_if(m_loadedPdfDocs.begin(), m_loadedPdfDocs.end(),
                                 [pDoc](const CPdfDoc *ptr) { return ptr == pDoc; });
        m_loadedPdfDocs.erase(it, m_loadedPdfDocs.end());
        delete pDoc; // Delete the destroyed document
    }
}

