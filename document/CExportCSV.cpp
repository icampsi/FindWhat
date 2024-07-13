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
#include "finder/CEsquema.h"
#include "utils/UText.h"
#include "utils/USerialize.h"

#include "ui/dialogs/ProgBarExport_dlg.h"
#include "ui/dialogs/InvalidFileName_dlg.h"


std::vector<std::vector<QString>> CExportCSV::convertModelToVector(QStandardItemModel* model) {
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    std::vector<std::vector<QString>> tableData(rowCount, std::vector<QString>(columnCount));

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QStandardItem* item = model->item(row, col);
            if (item) {
                tableData[row][col] = item->text();
            } else {
                tableData[row][col] = QString();
            }
        }
    }

    return tableData;
}

void CExportCSV::buildStructure(std::vector<std::vector<QString>> *xsvStructure, ProgBarExport_dlg *progressDialog, size_t maxColumns = 0) {
    CEsquema *esquema = m_associatedEsquemaDoc->getEsquema();
    std::vector<std::vector<QString>> format(convertModelToVector(&m_tableModel));

    // Ensure each row in format has at least maxColumns columns
    if (!format.empty() && format[0].size() < maxColumns) {
        for (std::vector<QString>& row : format) {
            row.resize(maxColumns, "");  // Add empty QStrings to the row if necessary
        }
    }

    int iteration{ 0 };
    for (QString& filePath : m_pdfFilePaths) {
        // Create the pdf document
        CPdfDoc* pdfDoc = nullptr;

        try {
            pdfDoc = new CPdfDoc(filePath); // Attempt to create pdfDoc
        }
        catch (const std::exception& e) {
            // If we fail to create document we simply go to the next one // BOOKMARK - Maybe come other mechanism would be better
            if (pdfDoc != nullptr) delete pdfDoc;
            QMessageBox::warning(nullptr, "Error", QString("Failed to process file: %1\nError: %2").arg(filePath, e.what()));
            continue;
        }

        // Continue if ID text not present
        if (!m_idText.isEmpty() && !pdfDoc->getFullText().contains(m_idText)) {
            delete pdfDoc;
            continue;
        }

        // Extract data
        std::unordered_map<QString, QString> extractedData;
        esquema->parseDoc(&pdfDoc->getPagedText(), &extractedData);

        auto replacer = [&extractedData](const QString& capturedString) -> QString {
            auto it = extractedData.find(capturedString);
            if (it != extractedData.end()) {
                return it->second;
            } else {
                return "ERROR_REPLACING_PLACEHOLDER";
            }
        };

        for (std::vector<QString> row : format) {
            for(QString& cell : row) {
                UText::replacePlaceholders(cell, "<(.*?)>", replacer);
            }
            xsvStructure->push_back(std::move(row));
        }

        // Rename document
        if (m_renameParsedPDFFlag) renameFile(filePath);
        // Update progress bar
        if (iteration % 2 == 0) progressDialog->updateProgress();

        ++iteration;
        delete pdfDoc;
    }
    progressDialog->updateProgress(); // Final update to ensure it doesn't get stuck on 99%
}

void CExportCSV::buildStructure(QStandardItemModel* combinedModel, ProgBarExport_dlg* progressDialog, size_t maxColumns) {
    CEsquema* esquema = m_associatedEsquemaDoc->getEsquema();
    std::vector<std::vector<QString>> format = convertModelToVector(&m_tableModel);

    // Ensure each row in format has at least maxColumns columns
    if (!format.empty() && format[0].size() < maxColumns) {
        for (std::vector<QString>& row : format) {
            row.resize(maxColumns, "");  // Add empty QStrings to the row if necessary
        }
    }

    int iteration = 0;
    for (const QString& filePath : m_pdfFilePaths) {
        // Create the pdf document
        CPdfDoc* pdfDoc = nullptr;

        try {
            pdfDoc = new CPdfDoc(filePath); // Attempt to create pdfDoc
        } catch (const std::exception& e) {
            // Handle creation failure
            if (pdfDoc != nullptr) delete pdfDoc;
            QMessageBox::warning(nullptr, "Error", QString("Failed to process file: %1\nError: %2").arg(filePath, e.what()));
            continue;
        }

        // Skip if ID text not present
        if (!m_idText.isEmpty() && !pdfDoc->getFullText().contains(m_idText)) {
            delete pdfDoc;
            continue;
        }

        // Extract data
        std::unordered_map<QString, QString> extractedData;
        esquema->parseDoc(&pdfDoc->getPagedText(), &extractedData);

        auto replacer = [&extractedData](const QString& capturedString) -> QString {
            auto it = extractedData.find(capturedString);
            if (it != extractedData.end()) {
                return it->second;
            } else {
                return "ERROR_REPLACING_PLACEHOLDER";
            }
        };

        // Add rows from the format to the combined model
        for (const std::vector<QString>& row : format) {
            QList<QStandardItem*> newRowItems;
            for (QString cell : row) {
                UText::replacePlaceholders(cell, "<(.*?)>", replacer);
                newRowItems.append(new QStandardItem(cell));
            }
            combinedModel->appendRow(newRowItems);
        }

        // Rename document
        if (m_renameParsedPDFFlag) renameFile(filePath);

        // Update progress bar
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

    QFile oldFile(oldFilePath);
    QFile newFile(newFilePath);
    if(oldFile.fileName() != newFile.fileName() && oldFile.rename(newFilePath)) {
        if (oldFile.rename(newFilePath)) {
            qDebug() << "File: " + oldFile.fileName() + " renamed successfully to: " + newFile.fileName();
        } else {
            qDebug() << "Failed to rename file:" << oldFile.errorString();
        }
    }
}

// SERIALIZATION
void serializeModel(std::ofstream &out, const QStandardItemModel* model) {
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    out.write(reinterpret_cast<const char*>(&rowCount), sizeof(int));
    out.write(reinterpret_cast<const char*>(&columnCount), sizeof(int));

    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            QStandardItem* item = model->item(row, column);
            USerialize::writeQString(out, item ? item->text() : QString());
        }
    }
}

void CExportCSV::serialize(std::ofstream &out) const {
    /* - SERIALIZATION ORDER -
     * std::vector<QString>    m_pdfFilePaths
     * QString                 m_exportFileRename
     * bool                    m_renameParsedPDFFlag
     * QString                 m_fileNamePlaceholder
     * size_t                  index of m_associatedEsquemaDoc
     * QString                 m_idText
     * QStandardItemModel      m_tableModel
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

    USerialize::writeQString(out, m_exportFileRename);                          // m_exportFileRename
    out.write(reinterpret_cast<const char*>(&m_renameParsedPDFFlag), sizeof(bool));     // m_renameParsedPDFFlag
    USerialize::writeQString(out, m_fileNamePlaceholder);                       // m_fileNamePlaceholder
    USerialize::writeQString(out, m_idText);                                    // m_idText

    // Serialize the QStandardItemModel
    serializeModel(out, &m_tableModel);
}

void deserializeModel(std::ifstream &in, QStandardItemModel* model) {
    int rowCount;
    int columnCount;

    in.read(reinterpret_cast<char*>(&rowCount), sizeof(int));
    in.read(reinterpret_cast<char*>(&columnCount), sizeof(int));

    model->setRowCount(rowCount);
    model->setColumnCount(columnCount);

    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            QString text;
            USerialize::readQString(in, text);
            QStandardItem* item = new QStandardItem(text);
            model->setItem(row, column, item);
        }
    }
}

void CExportCSV::deserialize(std::ifstream &in) {
    /* - SERIALIZATION ORDER -
     * std::vector<QString>    m_pdfFilePaths
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

    USerialize::readQString(in, m_exportFileRename);                 // m_exportFileRename
    in.read(reinterpret_cast<char*>(&m_renameParsedPDFFlag), sizeof(bool));  // m_renameParsedPDFFlag
    USerialize::readQString(in, m_fileNamePlaceholder);              // m_fileNamePlaceholder
    USerialize::readQString(in, m_idText);                           // m_idText

    // Deserialize the QStandardItemModel
    deserializeModel(in, &m_tableModel);
}
