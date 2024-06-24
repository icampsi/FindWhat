/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CMDoc.h"

#include "CPdfDoc.h"
#include "CEsquemaDoc.h"

#include <fstream>

CEsquemaDoc *CMDoc::newDoc(CEsquema *esquema) {
    CEsquemaDoc *pDoc = new CEsquemaDoc(esquema);
    m_loadedEsquemaDocs.push_back(pDoc);
    esquemaListUpdated();
    return pDoc;
}

CPdfDoc* CMDoc::newDoc(const QString& filePath) {
    CPdfDoc *pDoc = new CPdfDoc(filePath);
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

void CMDoc::deleteEsquema(const size_t index) {
    // Range Check
    if (index >= m_loadedEsquemaDocs.size()) {
        qDebug() << "Esquema out of range for deletition";
        return;
    }
    // Delete esquema
    qDebug() << "Deleting esquema: " << m_loadedEsquemaDocs[index]->getEsquema()->getName();
    delete m_loadedEsquemaDocs[index];
    m_loadedEsquemaDocs.erase(m_loadedEsquemaDocs.begin() + index);
    // Emit update esquema list
    esquemaListUpdated();
}

void CMDoc::reorderEsqDocs(const size_t objectToMoveIndex, const size_t destinationIndex) {
    if (objectToMoveIndex >= m_loadedEsquemaDocs.size() || destinationIndex >= m_loadedEsquemaDocs.size()) {
        qDebug() << "Invalid object index or destination index";
        return;
    }

    if (objectToMoveIndex == destinationIndex) {
        // No need to move if the indices are the same
        return;
    }

    if (objectToMoveIndex < destinationIndex) {
        // Move forward: Move the element at objectToMoveIndex to destinationIndex,
        // shifting the elements between them to the left
        std::rotate(m_loadedEsquemaDocs.begin() + objectToMoveIndex,
                    m_loadedEsquemaDocs.begin() + objectToMoveIndex + 1,
                    m_loadedEsquemaDocs.begin() + destinationIndex + 1);
    } else {
        // Move backward: Move the element at objectToMoveIndex to destinationIndex,
        // shifting the elements between them to the right
        std::rotate(m_loadedEsquemaDocs.begin() + destinationIndex,
                    m_loadedEsquemaDocs.begin() + objectToMoveIndex,
                    m_loadedEsquemaDocs.begin() + objectToMoveIndex + 1);
    }
}

// OBSERVER ARCHITECTURE
void CMDoc::esquemaListUpdated() {
    std::vector<QString> updatedEsquemaDocList;
    for (CEsquemaDoc *esquemaDoc : m_loadedEsquemaDocs) {
        updatedEsquemaDocList.push_back(esquemaDoc->getEsquema()->getName());
    }
    for (const auto& pair : m_esquemaDocObservers) {
        pair.second(updatedEsquemaDocList);
    }
}

size_t CMDoc::addObserver(std::function<void(const std::vector<QString>&)> observer) {
    m_esquemaDocObservers.emplace_back(m_nextHandle, observer);
    return m_nextHandle++;
}

void CMDoc::removeObserver(size_t handle) {
    auto it = std::remove_if(m_esquemaDocObservers.begin(), m_esquemaDocObservers.end(),
                             [&](const auto& pair) { return pair.first == handle; });
    m_esquemaDocObservers.erase(it, m_esquemaDocObservers.end());
}

// SERIALIZATOIN
void CMDoc::serializeFullEsquemaArray(std::ofstream& out) {
    // BOOKMARK - use template function but neeed to add serialization to esquema doc
    // SerializationUtils::writeCustomContainer(out, m_loadedEsquemaDocs);

    size_t loadedEsquemaDocsSize = m_loadedEsquemaDocs.size();
    out.write(reinterpret_cast<const char*>(&loadedEsquemaDocsSize), sizeof(size_t));

    for(CEsquemaDoc *esquemaDoc : m_loadedEsquemaDocs) {
        esquemaDoc->getEsquema()->serialize(out);
    }

    m_exportPathDoc.serialize(out);
}

void CMDoc::serializeEsquema(std::ofstream& out, CEsquemaDoc* esquemaDoc) {
    size_t loadedEsquemaDocsSize = 1;
    out.write(reinterpret_cast<const char*>(&loadedEsquemaDocsSize), sizeof(size_t));

    esquemaDoc->getEsquema()->serialize(out);
}


void CMDoc::deserializeSession(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs) {
    deserializeEsquema(in, loadedEsquemaDocs);
    m_exportPathDoc.deserialize(in);
}

void CMDoc::deserializeEsquema(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs) {
    size_t loadedEsquemaDocsSize;
    in.read(reinterpret_cast<char*>(&loadedEsquemaDocsSize), sizeof(size_t));
    for (size_t i{0}; i < loadedEsquemaDocsSize; i++) {
        CEsquema *esquema = new CEsquema(in);
        loadedEsquemaDocs.push_back(newDoc(esquema));
    }
}

