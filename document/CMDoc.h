/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CMDOC_H
#define CMDOC_H

#include <vector>
#include "CExportPathDoc.h"
#include "src/CEsquema.h"
#include "CDocument.h"

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
    CEsquemaDoc *getActiveEsquemaDoc() const { return m_activeEsquema; }
    CEsquemaDoc *getEsquemaFromIndex(int index) const { return m_loadedEsquemaDocs.at(index); }
    CPdfDoc     *getActivePdfDoc()     const { return m_activePdfDoc; }
    const std::vector<CEsquemaDoc*> *getLoadedEsquemaDocs() const { return &m_loadedEsquemaDocs; }

    void setActiveEsquemaDoc(CEsquemaDoc* esquema) { m_activeEsquema = esquema; }
    void setActivePdfDoc(CPdfDoc* pdfDoc) { m_activePdfDoc = pdfDoc; }

    CExportPathDoc& getExportPathDoc() { return m_exportPathDoc; }

    void deleteEsquema(size_t index);

    // SERIALIZATION
    void serializeFullEsquemaArray(std::ofstream& out);
    void serializeEsquema(std::ofstream& out, CEsquemaDoc* esquemaDoc);
    void deserialize(std::ifstream& in, std::vector<CEsquemaDoc*> &loadedEsquemaDocs);

    // No need for that until serialization is fully applied
    // void loadExportPathDoc(CExportPathDoc exportDoc) {
    //     m_exportPathDoc = exportDoc;
    // }

private:
    // PRIVATE SINGLETON CONSTRUCTOR
    CMDoc() : m_exportPathDoc(){}

    // MEMBERS
    std::vector<CEsquemaDoc*> m_loadedEsquemaDocs; // Vector of all loaded esquemeaDocs
    std::vector<CPdfDoc*>     m_loadedPdfDocs;     // Vector of all loaded PDF docs (it mainly holds the extracted text of those docs)
    CExportPathDoc            m_exportPathDoc;     // Class to hold the only exportPathDoc that should be loaded. It is created empty on the constructor of CMDoc

    CEsquemaDoc *m_activeEsquema = nullptr; // For easy and shared acces on the previewDocWindow only
    CPdfDoc     *m_activePdfDoc  = nullptr; // For easy and shared acces on the previewDocWindow only

    // Vector to store observer functions for Esquema document changes
    std::vector<std::function<void(const std::vector<QString>&)>> m_esquemaDocObservers;

    // OBSERVERS ARCHITECTURE
private:
    // Notify all registered Esquema document observers
    void esquemaListUpdated();

public:
    // Register and unregister observers for Esquema document changes
    void addObserver(std::function<void(const std::vector<QString>&)> observer) { m_esquemaDocObservers.push_back(observer); }
    void removeObserver(std::function<void(const std::vector<QString>&)> observer);
    void onDocumentDestroyed(CDocument *pDoc);
};

#endif // CMDOC_H
