/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CExportCSV.h"

#include <QDir>
#include <QFileInfo>

#include "CMDoc.h"
#include "CPdfDoc.h"
#include "CEsquemaDoc.h"
#include "src/CEsquema.h"
#include "utils/generalfunctions.h"

#include "ui/dialogs/ProgBarExport_dlg.h"
#include "ui/dialogs/InvalidFileName_dlg.h"

void CExportCSV::buildXSVStructure(std::vector<std::vector<QString>> *xsvStructure, ProgBarExport_dlg *progressDialog) {
    CEsquema *esquema = m_associatedEsquemaDoc->getEsquema();

    int iteration{ 0 };
    for (QString& filePath : m_pdfFilePaths) {
        CPdfDoc* pdfDoc = nullptr; // Initialize pointer to nullptr

        try {
            pdfDoc = new CPdfDoc(filePath); // Attempt to create pdfDoc
        }
        catch (const std::exception& e) {
            // Handle any exceptions that occurred during CPdfDoc construction
            delete pdfDoc; // Clean up allocated memory
            QMessageBox::warning(nullptr, "Error", QString("Failed to process file: %1\nError: %2").arg(filePath, e.what()));
            continue; // Move to the next iteration
        }

        // check if ID text is present, else break iteration
        if (!m_idText.isEmpty() && std::find(pdfDoc->getFullText().begin(), pdfDoc->getFullText().end(), m_idText) != pdfDoc->getFullText().end()) {
            delete pdfDoc;
            continue;
        }

        // Add extracted structure to the vector
        esquema->generateXSVStringStructure(pdfDoc);
        for (auto& j : esquema->getXSVStringStructureResult()) {
            xsvStructure->push_back(j);
        }

        // Rename document if flag enabled
        if (m_renameParsedPDFFlag) renameFile(filePath);
        // Update progress bar every other iteration to avoid overhead
        if (iteration % 2 == 0) progressDialog->updateProgress();

        ++iteration;
        delete pdfDoc;
    }
    progressDialog->updateProgress(); // Final update to ensure it doesn't get stuck on 99%
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
    if (!fileInfo.exists()) {
        qDebug() << "Source file does not exist:" << oldFilePath;
        return;
    }

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
        qDebug() << "File: " + file.fileName() + " renamed successfully to: " + newFileName;
    } else {
        qDebug() << "Failed to rename file:" << file.errorString();
    }
}

// SERIALIZATION
void CExportCSV::serialize(std::ofstream &out) const {
    /* - SERIALIZATION ORDER -
     * std::vector<QString>    m_pdfFilePaths
     * QString                 m_csvFormat;
     * QString                 m_exportFileRename
     * bool                    m_renameParsedPDFFlag
     * QString                 m_fileNamePlaceholder
     * size_t                  index of m_associatedEsquemaDoc
     * QString                 m_idText
     *
     * - NO NEED -
     * m_invalidFileNameDlg
     * m_pdfFilePaths
     */

    // Search associated esquema Index
    const std::vector<CEsquemaDoc*>* esquemaDocs = CMDoc::getMDoc().getLoadedEsquemaDocs();
    QString assocEsquemaName = m_associatedEsquemaDoc->getEsquema()->getName();

    // Use std::find_if with a lambda function to search for the object with m_name matching the searchString
    auto it = std::find_if(esquemaDocs->begin(), esquemaDocs->end(), [&assocEsquemaName](const CEsquemaDoc* obj) {
        return obj->getEsquema()->getName() == assocEsquemaName;
    });

    // Calculate the index
    size_t index = std::distance(esquemaDocs->begin(), it);
    out.write(reinterpret_cast<const char*>(&index), sizeof(size_t));  // m_associatedEsquemaDoc index

    SerializationUtils::writeQString(out, m_csvFormat);                                 // m_csvFormat
    SerializationUtils::writeQString(out, m_exportFileRename);                          // m_exportFileRename
    out.write(reinterpret_cast<const char*>(&m_renameParsedPDFFlag), sizeof(bool));     // m_renameParsedPDFFlag
    SerializationUtils::writeQString(out, m_fileNamePlaceholder);                       // m_fileNamePlaceholder
    SerializationUtils::writeQString(out, m_idText);                                    // m_idText
}

void CExportCSV::deserialize(std::ifstream &in) {
    /* - SERIALIZATION ORDER -
     * std::vector<QString>    m_pdfFilePaths
     * QString                 m_csvFormat;
     * QString                 m_exportFileRename
     * bool                    m_renameParsedPDFFlag
     * QString                 m_fileNamePlaceholder
     * QString                 m_idText

     * - NO NEED -
     * m_associatedEsquemaDoc
     * m_invalidFileNameDlg
     * m_pdfFilePaths
     */

    size_t index;
    in.read(reinterpret_cast<char*>(&index), sizeof(size_t));
    m_associatedEsquemaDoc = CMDoc::getMDoc().getEsquemaFromIndex(index);

    SerializationUtils::readQString(in, m_csvFormat);                        // m_csvFormat
    SerializationUtils::readQString(in, m_exportFileRename);                 // m_exportFileRename
    in.read(reinterpret_cast<char*>(&m_renameParsedPDFFlag), sizeof(bool));  // m_renameParsedPDFFlag
    SerializationUtils::readQString(in, m_fileNamePlaceholder);              // m_fileNamePlaceholder
    SerializationUtils::readQString(in, m_idText);                           // m_idText
}

