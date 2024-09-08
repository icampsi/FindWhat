#include "CParsedFile.h"

#include "CData.h"
#include "CFormula.h"
#include "CEsquema.h"

#include "poppler_interface/PdfToText.h"

bool CParsedFile::rename(const QString& newName) {
    QString directory = QFileInfo(m_filePath).absolutePath();
    QString newFilePath = directory + "/" + newName + ".pdf";

    qDebug() << "Renaming " << m_filePath << " to " << newFilePath;

    // Check if source file exists
    if (!QFile::exists(m_filePath)) {
        qDebug() << "Source file does not exist.";
        return false;
    }

    // Check if target file already exists
    if (QFile::exists(newFilePath)) {
        qDebug() << "Target file already exists.";
        return false;
    }

    // Attempt to rename the file
    QFile file(m_filePath);
    bool result = file.rename(newFilePath);

    // Log rename operation result
    if (!result) {
        qDebug() << "Rename failed:" << file.errorString();
    } else {
        qDebug() << "Rename succeeded";
    }

    return result;
}

bool CParsedFile::parseFileValues() {
    if(!m_assignedEsquema) {
        qDebug() << "No esquema assigned for file: " << m_filePath;
        return false;
    }
    CPagedText pagedText;
    Poppler_interface::loadPdfDocument(m_filePath, &pagedText);
    if(pagedText.isEmpty()) return false;

    // Add static data to the map
    for(const CData* data : (*m_assignedEsquema)->getStaticData()) {
        m_fieldValueMap.emplace(data->getDataName(), data->getDataString());
    }

    // Extract values for each formula and add it to the map
    for(CFormula* formula : (*m_assignedEsquema)->getExtractDataFormula()) {
        m_fieldValueMap.emplace(formula->getDataName(), formula->applyFormula(&pagedText).result);
    }

    return true;
}

const std::optional<QString> CParsedFile::getValue(const QString& fieldName) const {
    auto it = m_fieldValueMap.find(fieldName);
    if (it != m_fieldValueMap.end()) {
        return it.value();
    } else {
        qWarning() << "Esquema " << (*m_assignedEsquema)->getName() << " doesn't contain the field " << fieldName;
        return std::nullopt;  // No value found
    }
}
