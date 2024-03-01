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

// SERIALIZATOIN
void CMDoc::serialize(std::ofstream& out) {
    int loadedEsquemaDocsSize = m_loadedEsquemaDocs.size();
    out.write(reinterpret_cast<const char*>(&loadedEsquemaDocsSize), sizeof(int));

    for(CEsquemaDoc *esquemaDoc : m_loadedEsquemaDocs) {
        esquemaDoc->getEsquema()->serialize(out);
    }
}

void CMDoc::deserialize(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs) {
    int loadedEsquemaDocsSize;
    in.read(reinterpret_cast<char*>(&loadedEsquemaDocsSize), sizeof(int));
    for (int i{0}; i < loadedEsquemaDocsSize; i++) {
        CEsquema *esquema = new CEsquema(in);
        loadedEsquemaDocs.push_back(newDoc(esquema));
    }
}

void CMDoc::esquemaListUpdated() {
    std::vector<QString> updatedEsquemaDocList;
    for (auto& esquemaDoc : m_loadedEsquemaDocs) {
        updatedEsquemaDocList.push_back(esquemaDoc->getEsquema()->getName());
    }

    // Proceed with observer notification
    for (const auto &observer : m_esquemaDocObservers) {
        observer(updatedEsquemaDocList);
    }
}

void CMDoc::removeObserver(std::function<void(const std::vector<QString>&)> observer) {
    auto it = std::remove_if(m_esquemaDocObservers.begin(), m_esquemaDocObservers.end(),
                             [&](const std::function<void(const std::vector<QString>&)>& storedObserver) { return &observer == &storedObserver; }
                             );
    m_esquemaDocObservers.erase(it, m_esquemaDocObservers.end());
}

