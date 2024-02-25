#include "CExportCSV.h"
#include "qcoreapplication.h"
#include "src/CEsquema.h"
#include "utils/CTextExtractor.h"
#include "CDocumentSubclasses.h"


std::vector<std::vector<CData*>> CExportCSV::buildXSVStructure(exportCSVProgressBar_dlg *progressDialog) {
    std::vector<std::vector<CData*>> xsvStructure;
    QString text;
    CEsquema *esquema = m_associatedEsquemaDoc->getEsquema();

    for(auto& it : m_pdfFilePaths) {
        CTextExtractor::PDFToTextPoppler(it, text);
        esquema->generateXSVStructure(text);
        for(auto& j : esquema->getXSVStructureResult()) {
            xsvStructure.push_back(j);
        }
        QCoreApplication::processEvents();
        progressDialog->updateProgress();
    }

    return xsvStructure;
}

void CExportCSV::reOrderFiles(int fileToMoveIndex, int targetPositionIndex) {
    if (fileToMoveIndex < 0 || fileToMoveIndex >= m_pdfFilePaths.size() ||
        targetPositionIndex < 0 || targetPositionIndex >= m_pdfFilePaths.size())
        return;

    std::rotate(m_pdfFilePaths.begin() + fileToMoveIndex,
                m_pdfFilePaths.begin() + fileToMoveIndex + (fileToMoveIndex < targetPositionIndex ? 1 : 0),
                m_pdfFilePaths.begin() + targetPositionIndex + (fileToMoveIndex < targetPositionIndex ? 1 : 0)
                );
}