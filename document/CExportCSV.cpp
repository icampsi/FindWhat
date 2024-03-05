#include "CExportCSV.h"

#include "qcoreapplication.h"
#include "qdir.h"
#include "qfileinfo.h"

#include "qregularexpression.h"
#include "src/CEsquema.h"
#include "utils/CTextExtractor.h"
#include "CDocumentSubclasses.h"


void CExportCSV::buildXSVStructure(std::vector<std::vector<QString>> &xsvStructure, ExportCSVProgressBar_dlg *progressDialog) {
    QString text;
    CEsquema *esquema = m_associatedEsquemaDoc->getEsquema();

    for(auto& it : m_pdfFilePaths) {
        CTextExtractor::PDFToTextPoppler(it, text);
        esquema->generateXSVStringStructure(text);
        for(auto& j : esquema->getXSVStringStructureResult()) {
            xsvStructure.push_back(j);
        }
        if(m_renameParsedPDFFlag) renameFile(it); // Rename document if flag enabled

        // Update progress bar
        QCoreApplication::processEvents();
        progressDialog->updateProgress();
    }
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
    if (!m_associatedEsquemaDoc->getEsquema()->createFileName(newFileName, m_fileNamePlaceholder)) {
        m_invalidFileNameDlg = new InvalidFileName_dlg(nullptr, &newFileName, oldFilePath);
        if (m_invalidFileNameDlg->exec() == QDialog::Rejected) {
            return;
        }
    }

    QString newFilePath = QDir::toNativeSeparators(fileInfo.path() + QDir::separator() + newFileName + '.' + fileInfo.suffix());

    QFile file(oldFilePath);
    if (file.rename(newFilePath)) {
        qDebug() << "File renamed successfully.";
    } else {
        qDebug() << "Failed to rename file:" << file.errorString();
    }
}
