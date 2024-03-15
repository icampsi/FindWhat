// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "CMDoc.h"

#include "CPdfDoc.h"
#include "CEsquemaDoc.h"

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

CMDoc& CMDoc::getMDoc() {
    static CMDoc instance; // Initialized on first use
    return instance;
}

CMDoc::~CMDoc() {
    for (auto* esquemaDoc : m_loadedEsquemaDocs) delete esquemaDoc;
    m_loadedEsquemaDocs.clear();
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

void CMDoc::deleteEsquema(int index) {
    if (index < 0 || index >= m_loadedEsquemaDocs.size()) {
        qDebug() << "Esquema out of range for deletition";
        return;
    }
    delete m_loadedEsquemaDocs[index];
    m_loadedEsquemaDocs.erase(m_loadedEsquemaDocs.begin() + index);
}


// SERIALIZATOIN
void CMDoc::serializeFullEsquemaArray(std::ofstream& out) {
    size_t loadedEsquemaDocsSize = m_loadedEsquemaDocs.size();
    out.write(reinterpret_cast<const char*>(&loadedEsquemaDocsSize), sizeof(size_t));

    for(CEsquemaDoc *esquemaDoc : m_loadedEsquemaDocs) {
        esquemaDoc->getEsquema()->serialize(out);
    }
}

void CMDoc::serializeEsquema(std::ofstream& out, CEsquemaDoc* esquemaDoc) {
    size_t loadedEsquemaDocsSize = 1;
    out.write(reinterpret_cast<const char*>(&loadedEsquemaDocsSize), sizeof(size_t));

    esquemaDoc->getEsquema()->serialize(out);
}


void CMDoc::deserialize(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs) {
    size_t loadedEsquemaDocsSize;
    in.read(reinterpret_cast<char*>(&loadedEsquemaDocsSize), sizeof(size_t));
    for (size_t i{0}; i < loadedEsquemaDocsSize; i++) {
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

