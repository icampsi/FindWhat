/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CExportCSV.h"

#include "qcoreapplication.h"
#include "qdir.h"
#include "qfileinfo.h"

#include "src/CEsquema.h"
#include "CPdfDoc.h"
#include "CEsquemaDoc.h"

void CExportCSV::buildXSVStructure(std::vector<std::vector<QString>> &xsvStructure, ProgBarExport_dlg *progressDialog) {
    CEsquema *esquema = m_associatedEsquemaDoc->getEsquema();

    for(QString& filePath : m_pdfFilePaths) {
        CPdfDoc* pdfDoc = new CPdfDoc(filePath);
        esquema->generateXSVStringStructure(pdfDoc->getFullText());
        for(auto& j : esquema->getXSVStringStructureResult()) {
            xsvStructure.push_back(j);
        }
        if(m_renameParsedPDFFlag) renameFile(filePath); // Rename document if flag enabled

        // Update progress bar
        QCoreApplication::processEvents();
        progressDialog->updateProgress();
        delete pdfDoc;
    }
}

void CExportCSV::reOrderFiles(size_t fileToMoveIndex, size_t targetPositionIndex) {
    if (fileToMoveIndex >= m_pdfFilePaths.size() || targetPositionIndex >= m_pdfFilePaths.size())
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
