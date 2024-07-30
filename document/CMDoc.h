/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CMDOC_H
#define CMDOC_H

#include "CExportPathDoc.h"
#include "document/CEsquemaDoc.h"
#include "finder/CEsquema.h"
#include "CDocument.h"
#include <vector>

class CDocument;
class CPdfDoc;

class CMDoc { // Singleton class to hold and acces the document classes all together
public:
    // INDIRECT SINGLETON CONSTRUCTOR AND DESTURCTOR
    static CMDoc& getMDoc();
    ~CMDoc();

    // Delete copy constructor and assignment operator to prevent cloning and assignment
    CMDoc(const CMDoc&)          = delete;
    void operator=(const CMDoc&) = delete;

    // Function to create a new doc (depending on overload parameters), store it into the array and return a pointer to it.
    CEsquemaDoc *newDoc(CEsquema* esquema);
    CPdfDoc     *newDoc(const QString& filePath);

    // GETTERS AND SETTERS
    CEsquemaDoc *getActiveEsquemaDoc() const                        { return m_activeEsquema; }
    CEsquemaDoc *getEsquemaFromIndex(const size_t index) const      { return m_loadedEsquemaDocs.at(index); }
    CPdfDoc     *getActivePdfDoc() const                            { return m_activePdfDoc; }
    const std::vector<CEsquemaDoc*> *getLoadedEsquemaDocs() const   { return &m_loadedEsquemaDocs; }

    void setActiveEsquemaDoc(CEsquemaDoc* esquema) { m_activeEsquema = esquema; }
    void setActivePdfDoc(CPdfDoc* pdfDoc) { m_activePdfDoc = pdfDoc; }

    CExportPathDoc& getExportPathDoc() { return m_exportPathDoc; }

    void deleteEsquema(const size_t index);
    void deleteEsquema(CEsquemaDoc* docToDelete);

    void reorderEsqDocs(const size_t objectToMoveIndex, const size_t destinationIndex);

    void onDocumentDestroyed(CDocument *pDoc);

    // SERIALIZATION
    void serializeFullEsquemaArray(std::ofstream& out);
    void serializeEsquema(std::ofstream& out, CEsquemaDoc* esquemaDoc);
    void deserializeSession(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs);
    void deserializeEsquema(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs);

private:
    // PRIVATE SINGLETON CONSTRUCTOR
    CMDoc() : m_exportPathDoc(){}

    // MEMBERS
    std::vector<CEsquemaDoc*> m_loadedEsquemaDocs; // Vector of all loaded esquemeaDocs
    std::vector<CPdfDoc*>     m_loadedPdfDocs;     // Vector of all loaded PDF docs (it mainly holds the extracted text of those docs)
    CExportPathDoc            m_exportPathDoc;     // Class to hold the only exportPathDoc that should be loaded. It is created empty on the constructor of CMDoc

    CEsquemaDoc *m_activeEsquema = nullptr; // For easy and shared acces on the previewDocWindow only
    CPdfDoc     *m_activePdfDoc  = nullptr; // For easy and shared acces on the previewDocWindow only

private:
    // OBSERVERS ARCHITECTURE
    // Vector to store observer functions for Esquema document changes
    std::vector<std::pair<size_t, std::function<void(const std::vector<QString>&)>>> m_esquemaDocObservers;
    size_t m_nextHandle = 0;
    // Notify all registered Esquema document observers
    void esquemaListUpdated();

public:
    // Register and unregister observers for Esquema document changes
    size_t addObserver(std::function<void(const std::vector<QString>&)> observer);
    void removeObserver(size_t handle);
};

#endif // CMDOC_H
