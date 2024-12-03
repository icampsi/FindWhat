/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CExportCSV.h"

#include <QDir>
#include <QFileInfo>

#include "CMDoc.h"
#include "CEsquemaDoc.h"
#include "CParsedPdfModel.h"
#include "finder/CEsquema.h"
#include "utils/UText.h"
#include "utils/USerialize.h"

#include "ui/dialogs/ProgBar_dlg.h"
#include "ui/dialogs/InvalidFileName_dlg.h"

#include "../poppler_interface/PdfToText.h"

#ifdef ENABLE_DBMANAGER
#include "dbManager/CDbConnection.h"
#include "dbManager/sql_queries.h"
#endif

CExportCSV::CExportCSV()
    : m_associatedEsquema(nullptr),
    m_exportFileRename(),
    m_renameParsedPDFFlag(false),
    m_fileNamePlaceholder(),
    m_idText(""),
    m_csvTableModel(10, 10)
#ifdef ENABLE_DBMANAGER
    , m_dbTableModel()  // Table model for the db format table
#endif
{
#ifdef ENABLE_DBMANAGER
    // QString query = SqlQueries::GetRuBills + " LIMIT 0";
    // m_dbTableModel.setQuery(query, QSqlDatabase::database("closca"));
    // m_dbTableModel.insertRows(0, 1);

    setupDbFormatTable();
#endif
}

#ifdef ENABLE_DBMANAGER
void CExportCSV::setupDbFormatTable() {
    const QVector<QString> colNames {
        "Bill Number",
        "Expedition Date",
        "Billing Periode Start",
        "Billing Periode End",
        "Company CIF",
        "Floor",
        "Door",
        "Utility Name",
        "IVA",
        "BI",
        "Total"
    };
    if(m_dbTableModel.rowCount() < 1) {
        m_dbTableModel.insertRows(0, 1);
    }
    m_dbTableModel.setHorizontalHeaderLabels(colNames);
}
#endif

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

bool CExportCSV::parseFileValues(int index) {
    return m_files[index].parseFileValues();
}

void CExportCSV::buildStructure(CParsedPdfModel* combinedModel, ProgBar_dlg* progressDialog, bool CSVParser) {
    int iteration = 0;
    int firstEmptyRow = combinedModel->rowCount(); // To track where to start inserting data
    QAbstractItemModel *pFormatModel = nullptr; // will refer to m_dbTableModel or m_csvTableModel depending on the used parser
    if(!CSVParser) {
#ifdef ENABLE_DBMANAGER
        pFormatModel = &m_dbTableModel;
#endif
    } else {
        pFormatModel = &m_csvTableModel;
    }

    combinedModel->insertRows(combinedModel->rowCount(), m_files.size()); // Insert enough blanck rows to the model

    if(!m_files.isEmpty()) {
        // Add columns if the table model is bigger than the previous ones
        if(combinedModel->columnCount() < pFormatModel->columnCount()) {
            int colDiff = pFormatModel->columnCount() - combinedModel->columnCount();
            combinedModel->insertColumns(combinedModel->columnCount(), colDiff);
        }
#ifdef ENABLE_DBMANAGER
        // Add column name as header data for dbParser
        if(!CSVParser) {
            for(int i{0}; i < pFormatModel->columnCount(); ++i) {
                combinedModel->setHeaderData(i, Qt::Horizontal, pFormatModel->headerData(i, Qt::Horizontal));
            }
        }
#endif
    }

    // Iterate through all m_files
    for (int i{0}; i < m_files.size(); ++i) {
        CParsedFile& file = m_files[i];
        // Parse values
        file.parseFileValues();

        {
            // REPLACE FROMAT PLACEHOLDERS FOR EXTRACTED DATA AND APPEND TO COMBINEDMODEL
            auto replacer = [&](const QString& capturedString) -> const std::optional<QString> {
                return file.getValue(capturedString);
            };

            // Iterate over every column in the format model to insert the correponding data in each cell
            for (int j = 0; j < pFormatModel->columnCount(); ++j) {
                QModelIndex index = pFormatModel->index(0, j);
                QString data = pFormatModel->data(index).toString();
                UText::replacePlaceholders(data, "<(.*?)>", replacer);

                if(!combinedModel->setData(combinedModel->index(iteration + firstEmptyRow, j), data)) {
                    qWarning() << "CExportCSV::buildStructure:\nError setting data for row " << iteration << " and column " << j;
                }
            }
        }

        // Add file path as metadata on each row of the table
        combinedModel->setFileMetadata(i, &m_files[i]);

        // RENAME DOCUMENT
        if(m_renameParsedPDFFlag) {
         renameFile(m_files[i].filePath());
        }

        // UPDATE PROGRESSBAR
        if (iteration % 2 == 0) progressDialog->updateProgress();
        ++iteration;
    }
    progressDialog->updateProgress(); // Final update to ensure progressbar doesn't get stuck at 99%
}

const QVector<CParsedFile>* CExportCSV::setFiles(const std::vector<QString>& paths) {
    for (const QString& filePath : paths) {
        CParsedFile file(filePath, &m_associatedEsquema);
        file.parseFileValues();
        m_files.push_back(std::move(file));
    }
    return &m_files;
}

void CExportCSV::renameFile(const QString &oldFilePath) {
    QFileInfo fileInfo(oldFilePath);
    if (!fileInfo.exists()) {
        qWarning() << "Source file does not exist:" << oldFilePath;
        return;
    }

    QString newFileName;
    if (!m_associatedEsquema->createFileName(newFileName, m_fileNamePlaceholder)) {
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
            qWarning() << "File: " + oldFile.fileName() + " renamed successfully to: " + newFile.fileName();
        } else {
            qWarning() << "Failed to rename file:" << oldFile.errorString();
        }
    }
}

// SERIALIZATION
void CExportCSV::serialize(std::ofstream &out) const {
    /* - SERIALIZATION ORDER -
     * QString                 m_exportFileRename
     * bool                    m_renameParsedPDFFlag
     * QString                 m_fileNamePlaceholder
     * size_t                  index of m_associatedEsquemaDoc
     * QString                 m_idText
     * QStandardItemModel      m_csvTableModel
     * CRecModel               m_dbTableModel
     *
     * - NO NEED -
     * m_invalidFileNameDlg
     * m_pdfFilePaths
     */

    // Search associated esquema Index
    const std::vector<CEsquemaDoc*>* esquemaDocs = CMDoc::getMDoc().getLoadedEsquemaDocs();
    QString assocEsquemaName = m_associatedEsquema->getName();

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

    // Serialize the m_csvTableModel (QStandarditemModel)
    USerialize::writeModel(out, &m_csvTableModel);

#ifdef ENABLE_DBMANAGER
    // Serialize m_dbTableModel (QStandarditemModel)
    USerialize::writeModel(out, &m_dbTableModel);
#endif
}

void CExportCSV::deserialize(std::ifstream &in) {
    /* - SERIALIZATION ORDER -
     * QString                 m_exportFileRename
     * bool                    m_renameParsedPDFFlag
     * QString                 m_fileNamePlaceholder
     * QString                 m_idText
     * QStandardItemModel      m_csvTableModel
     * CRecModel               m_dbTableModel

     * - NO NEED -
     * m_associatedEsquemaDoc
     * m_invalidFileNameDlg
     * m_pdfFilePaths
     */

    {
        size_t index;
        in.read(reinterpret_cast<char*>(&index), sizeof(size_t));
        m_associatedEsquema = CMDoc::getMDoc().getEsquemaFromIndex(index)->getEsquema();
    }
    USerialize::readQString(in, m_exportFileRename);                 // m_exportFileRename
    in.read(reinterpret_cast<char*>(&m_renameParsedPDFFlag), sizeof(bool));  // m_renameParsedPDFFlag
    USerialize::readQString(in, m_fileNamePlaceholder);              // m_fileNamePlaceholder
    USerialize::readQString(in, m_idText);                           // m_idText

    // Deserialize the QStandardItemModel
    USerialize::readModel(in, &m_csvTableModel);

#ifdef ENABLE_DBMANAGER
    USerialize::readModel(in, &m_dbTableModel);
#endif

}
