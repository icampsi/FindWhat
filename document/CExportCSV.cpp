/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CExportCSV.h"

#include <QDir>
#include <QFileInfo>

#include "CMDoc.h"
#include "CEsquemaDoc.h"
#include "finder/CEsquema.h"
#include "utils/UText.h"
#include "utils/USerialize.h"

#include "ui/dialogs/ProgBarExport_dlg.h"
#include "ui/dialogs/InvalidFileName_dlg.h"

#include "../poppler_interface/PdfToText.h"

void CExportCSV::convertModelToVector(QAbstractItemModel* model, std::vector<std::vector<QString>>* format) {
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    format->clear();
    format->reserve(rowCount);  // Reserve space for rows

    for (int row = 0; row < rowCount; ++row) {
        // Reserve space for columns
        std::vector<QString> rowVector;
        rowVector.reserve(columnCount);

        for (int col = 0; col < columnCount; ++col) {
            QModelIndex index = model->index(row, col);
            QVariant data = model->data(index);
            QString value = data.isValid() ? data.toString() : QString();
            rowVector.emplace_back(std::move(value));
        }

        format->emplace_back(std::move(rowVector));
    }
}



void CExportCSV::buildStructure(QStandardItemModel* combinedModel, ProgBarExport_dlg* progressDialog, size_t maxColumns, bool dbParser) {
    CEsquema* esquema = m_associatedEsquemaDoc->getEsquema();

    int iteration = 0;
    for (const QString& filePath : m_pdfFilePaths) {
        // EXTRACT TEXT FROM PDF
        CPagedText pagedText;
        Poppler_interface::loadPdfDocument(filePath, &pagedText);
        if(pagedText.isEmpty()) continue;

        // EXTRACT DATA FROM TEXT
        std::unordered_map<QString, QString> extractedData;
        esquema->parseDoc(&pagedText, &extractedData);

        // SETUP FORMAT
        // Define the format for the table
        std::vector<std::vector<QString>> format;
        if(!dbParser) {
            convertModelToVector(&m_csvTableModel, &format);
        } else {
#ifdef ENABLE_DBMANAGER
            // Note: m_csvTableModel has rows where columns should be!
            int rowCount = m_dbTableModel.rowCount();
            format.reserve(1);  // Reserve space for rows

            // Reserve space for columns
            std::vector<QString> rowVector;
            rowVector.reserve(rowCount);
            std::vector<QString> header;
            header.reserve(rowCount);

            for (int col = 0; col < rowCount; ++col) {
                QString value = m_dbTableModel.value(col).toString();
                if(combinedModel->rowCount() <= 0) {
                    QString fieldName = m_dbTableModel.fieldName(col).toString();
                    header.emplace_back(std::move(fieldName));
                }
                rowVector.emplace_back(std::move(value));
            }
            // First time, append header
            if(combinedModel->rowCount() <= 0) {
                QList<QStandardItem*> newHeaderItems;
                for (const QString& cell : header) {
                    newHeaderItems.append(new QStandardItem(cell));
                }
                combinedModel->appendRow(newHeaderItems);
            }

            format.emplace_back(std::move(rowVector));
#endif
        }

        // Ensure each row in format has at least maxColumns columns
        if (!format.empty() && format[0].size() < maxColumns) {
            for (std::vector<QString>& row : format) {
                row.resize(maxColumns, "");  // Add empty QStrings to the row if necessary
            }
        }

        // REPLACE FROMAT PLACEHOLDERS FOR EXTRACTED DATA AND APPEND TO COMBINEDMODEL
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
    }
    progressDialog->updateProgress(); // Final update to ensure progressbar doesn't get stuck at 99%
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

    // Search for the object with m_name matching the searchString
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
    serializeModel(out, &m_csvTableModel);
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
    deserializeModel(in, &m_csvTableModel);
}
