#include "CExportCSV.h"

#include "qcoreapplication.h"
#include "qdir.h"
#include "qfileinfo.h"

#include "src/CEsquema.h"
#include "utils/CTextExtractor.h"
#include "CDocumentSubclasses.h"


std::vector<std::vector<CData*>> CExportCSV::buildXSVStructure(ExportCSVProgressBar_dlg *progressDialog) {
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
        renameFile(it);
        // NEED TO RENAME THE FILES HERE MAYBE??
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

void CExportCSV::renameFile(const QString &oldFilePath) {
    QFileInfo fileInfo(oldFilePath);
    QString newFileName;
    m_associatedEsquemaDoc->getEsquema()->createFileName(newFileName);
    QString newFilePath = QDir::toNativeSeparators(fileInfo.path() + QDir::separator() + newFileName + '.' + fileInfo.suffix());

    QFile file(oldFilePath);
    if (file.rename(newFilePath)) {
        qDebug() << "File renamed successfully.";
    } else {
        qDebug() << "Failed to rename file:" << file.errorString();
    }
}
